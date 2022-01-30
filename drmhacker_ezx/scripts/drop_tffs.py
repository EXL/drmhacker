#!/usr/bin/env python

import sys

def decode_general(sFileName, sMagic):
	with open(sFileName, 'rb') as file, open(sFileName + '_converted', 'wb') as output:
		data = file.read()
		offset = data.find(sMagic)
		print('Info: Data offset is: ' + '0x{:08X}'.format(offset) + '.\n')
		print('=== START CHECKSUM DATA ====')
		file.seek(offset)
		count = 0
		while True:
			bytes = file.read(0x400)
			output.write(bytes)
			chksum = file.read(8)
			if not bytes or not chksum:
				break
			count += 1
			print('0x{:08X}: '.format(0x400 * count) + chksum.hex(' ').upper() + ' ' + '0x{:08X}'.format(count * 2))
		print('=== END CHECKSUM DATA ====')

def decode_CG35(sFileName):
	print('Info: CG35 decoding.')
	decode_general(sFileName, b'\x45\x3D\xCD') # CramFS Image Magic.

def decode_CG38(sFileName):
	print('Info: CG38 decoding.')
	decode_general(sFileName, b'\xEB\x3C\x90') # FAT Image Magic.

def main(sFileName):
	if 'CG35' in sFileName:
		decode_CG35(sFileName)
	elif 'CG38' in sFileName:
		decode_CG38(sFileName)
	else:
		print('Error: The filename does not contain the CG35 or CG38 words!')

if __name__ == '__main__':
	print('Convert CG35 and CG38 code groups of Motorola E680, E680i, E680g, and A780 phones to a readable files.')
	print('Version: 1.0, (c) EXL, 29-Jan-2022\n')
	if len(sys.argv) != 2:
		print('Usage:')
		print('\t./drop_tffs.py CG35.smg')
		print('\t./drop_tffs.py CG38.smg')
		exit(1)
	main(sys.argv[1])
