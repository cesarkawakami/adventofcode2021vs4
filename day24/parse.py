import sys
import re

prog_chunks = []

current_chunk = ''
for line in sys.stdin:
	line = line.strip()
	if not line.strip() or line.startswith('#'):
		continue
	if line.startswith('inp') and current_chunk:
		prog_chunks.append(current_chunk)
		current_chunk = ''
	current_chunk += line + '\n'

if current_chunk:
	prog_chunks.append(current_chunk)

REGEX = r'''^
inp w
mul x 0
add x z
mod x 26
div z (-?\d+)
add x (-?\d+)
eql x w
eql x 0
mul y 0
add y 25
mul y x
add y 1
mul z y
mul y 0
add y w
add y (-?\d+)
mul y x
add z y
$'''

REGEX = REGEX.replace(' ', r'\s*').replace('\n', r'\s*')

for index, chunk in enumerate(prog_chunks):
	m = re.match(REGEX, chunk, re.MULTILINE)
	if not m:
		print('not matched!!!!')
		print(chunk)
		sys.exit(1)
	a, b, c = map(int, m.groups())
	print(f'{{{a: 3d},{b: 4d},{c: 3d}}},')
    # print(f'{index: 3d}: ({a: 3d},{b: 4d},{c: 3d})')
