#!/usr/bin/python3

import socket
import random
import string
import time
import threading
import getopt
import sys
import datetime

needProgress = False
debug = False
numberOfString = 1

def getTime():
	t = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
	return t

'''
参数：主机，端口，随机发送数据最小长度(*32)，随机发送数据最大长度(*32)
返回：测试结果，发送字符串，接收字符串，接收字符串耗时
'''
def sendAndRecvData(host, port, minDataLen, maxDataLen):
	try:
		sk = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sk.connect((host, port))
	except Exception as e:
		if debug == True:
			print(getTime(), "Excetion connect: ", e)
		return False, "", "", -1

	isOk = True
	cost = 0
	for i in range(0, numberOfString):
		sendStr = ''
		for i in range(random.randint(minDataLen, maxDataLen)):
			sendStr = sendStr + ''.join(random.sample(string.ascii_letters + string.digits, 32))
		sendStr = sendStr + '\n'

		try:
			sk.sendall((sendStr).encode("utf8"))
			# sk.shutdown(socket.SHUT_WR)
		except Exception as e:
			if debug == True:
				print(getTime(), "Excetion send: ", e)
			return False, sendStr, "", -2

		time_start=time.time()
		recvLen = 0
		recvStr = ''
		try:
			while recvLen < len(sendStr):
				tmpStr = sk.recv(4096).decode('UTF-8')
				recvLen += len(tmpStr)
				recvStr = recvStr + tmpStr
		except Exception as e:
			if debug == True:
				print(getTime(), "Excetion recv: ", e)
			return False, sendStr, recvStr, -3
		time_end=time.time()

		cost = cost + (time_end-time_start)
		isOk = recvStr==sendStr
		if isOk == False:
			break

	try:
		sk.close()
	except Exception as e:
		if debug == True:
			print(getTime(), "Excetion close: ", e)
		return False, sendStr, recvStr, -4

	return isOk, sendStr, recvStr, cost

class testThread(threading.Thread):
	"""测试线程 参数为子线程号，线程中的循环次数"""
	def __init__(self, host, port, threadNum, repeatCount, minDataLen, maxDataLen):
		super(testThread, self).__init__()
		self.threadNum = threadNum
		self.repeatCount = repeatCount
		self.minDataLen =  minDataLen
		self.maxDataLen =  maxDataLen
		self.host = host
		self.port = port
		self.threadTestResult = []

	def run(self):
		global needProgress
		for i in range(1, self.repeatCount+1):
			if needProgress:
				print(getTime(), "Child number: ", self.threadNum, "\t Repeat time: ", i)
			testResult, sendStr, recvStr, cost = sendAndRecvData(self.host, self.port, self.minDataLen, self.maxDataLen);
			# print("Child number: ", self.threadNum, '\t Result: ', testResult, " Cost: ", cost)

			# 保存测试结果
			result = [self.threadNum, i]
			result.append(testResult)
			result.append(sendStr)
			result.append(recvStr)
			result.append(cost)
			self.threadTestResult.append(result)

	def getThreadTestResult(self):
		return self.threadTestResult

'''
参数：主机，端口，子线程编号，线程循环次数，随机发送数据最小长度(*32)，随机发送数据最大长度(*32)
'''
def doTest(host, port, chlidNum, repeatCount, minDataLen, maxDataLen):
	threads = []
	try:
		for i in range(0, chlidNum):
			thread = testThread(host, port, i, repeatCount, minDataLen, maxDataLen)
			thread.start()
			threads.append(thread)

		for t in threads:
			t.join()

		print("线程号\t循环号\t结果\t接收耗时")
		totalSuccess = 0
		totalCost = 0;
		for t in threads:
			for r in t.getThreadTestResult():
				print(r[0], "\t", r[1], "\t", r[2], "\t", r[5])
				if r[2]==True:
					totalSuccess += 1
					totalCost += r[5]
		successRate = totalSuccess/(chlidNum*repeatCount)*100;
		timePerRequest = totalCost/totalSuccess if totalSuccess>0 else 0
		print("成功率: %.2f%%\t总耗时: %fs\t平均耗时:%fs"%(successRate, totalCost, timePerRequest))
	except Exception as e:
		if debug == True:
			print("Excetion create thread: ", e)

def usage():
	print('''
usage: python3 SocketTest.py
		[-p(progress)] [-d(debug)] [-n(number of string)]
		<host> <port>
		<thread count> <repeat count>
		<min data len> [<max data len>]
		''')

def main():
	global needProgress
	global debug
	global numberOfString
	try:
		opts, args = getopt.getopt(sys.argv[1:], "phdn:", ["progress","help","debug", "number"])
	except Exception as e:
		print(e)
		usage()
		sys.exit(2)

	for o, a in opts:
		if o in ("-h", "--help"):
			usage();
			sys.exit(0)
		elif o in ("-p", "--progress"):
			needProgress = True
		elif o in ("-d", "--debug"):
			debug = True
		elif o in ("-n", "--number"):
			numberOfString = int(a)
		else:
			assert False, "unhandled option"

	if len(args)==5:
		args.append(args[4])

	if len(args)!=6:
		print("invalid option")
		usage()
		sys.exit(2)

	doTest(args[0], int(args[1]), int(args[2]), int(args[3]), int(args[4]), int(args[5]))

if __name__ == '__main__':
	main()
