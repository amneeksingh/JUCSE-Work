import socket

# Accept ip address 
port=int(input("Enter port of server machine "))

sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

sock.bind(('', port))
sock.listen(5)
c, addr=sock.accept()

filename=c.recv(1024).decode()

with open(filename,'rb') as f:

	print('Sending file..')
	# Sending file line by line
	lines=f.read(1024)
	while(lines):
		c.send(lines)
		lines=f.read(1024)


# c.send('#'.encode())
print('File tranfer complete')
c.shutdown(socket.SHUT_WR)

