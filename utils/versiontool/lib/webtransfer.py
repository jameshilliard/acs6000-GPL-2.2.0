#!/usr/bin/python
##
# This program is a webtransfer by socket scripts.
#
# Author:Tercel Yi
# History: 
#	create file on 2007/11/19
#
# @Copyright Avocent

from socket import *
from urlparse import urlparse

class WebTransfer:
	def __init__ (self, maxlen) :
		self.maxlen = maxlen	
		
	def recvBasic(self,the_socket) :
		total_data=[]
		
		while True:
			data = the_socket.recv(self.maxlen)
			if not data: break
			total_data.append(data)
		return ''.join(total_data)

	def getSocketResult(self, host, port, sendinfo) :
		if port == None :
			port = 80
		if sendinfo=='' :
			sendinfo = '/'
		sock = socket(AF_INET,SOCK_STREAM)
		#sock.setblocking(1)
		sock.connect((host,port))
		sock.send('GET '+sendinfo+'\n')
		return self.recvBasic(sock)
		
def getWebContent(url_str,maxlen) :
	url =  urlparse(url_str)
	C_wt =WebTransfer(maxlen)
	return C_wt.getSocketResult(url.hostname,url.port,url.path)

	



