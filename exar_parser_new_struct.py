import os
import datetime
import sys

try:
	filename = sys.argv[1]
except:
	print('Wrong file path')
	sys.exit()

payload = []

modified = datetime.datetime.fromtimestamp(os.stat(filename).st_mtime).strftime('%Y-%m-%d %H:%M:%S')
print('Parsing file {}, modified at {}'.format(filename, modified))

with open(filename, 'r') as f:
	line = f.readline()
	while line:
		try:
			length = int(line[1:3], 16)
			if length > 0:
				address = int(line[3:7], 16)
				print(address);
				data = line[9:9 + length * 2]
				
				for i in range(0, len(data) - 1, 2):
					payload.append((hex(address), data[i:i+2]))
					address += 1
		except:
			pass

		line = f.readline()

result = 'typedef struct {{ uint16_t address; uint8_t data; }} xr_data;\r\n xr_data {name}[{len}] = {{ {dump} }};'.format(name="_".join(['xr77129', filename.split('.')[0].lower(), 'cfg']), len=len(payload), dump=", ".join(['{{ {}, 0x{} }}'.format(i[0], i[1]) for i in payload]))
print(result)