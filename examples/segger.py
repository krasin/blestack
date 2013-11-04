# coding: utf-8

# The MIT License (MIT)
#
# Copyright (c) 2013 Paulo Sérgio Borges de Oliveira Filho
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import subprocess
import argparse
import os.path
import os
import sys

###############################################################################

parser = argparse.ArgumentParser(prog="segger")
subparsers = parser.add_subparsers()

erase = subparsers.add_parser("erase", help="erase the flash")
erase.set_defaults(command="erase")

flash = subparsers.add_parser("flash", help="program the flash")
flash.set_defaults(command="flash")
flash.add_argument("program", help="binary file containing the program")

softdevice = subparsers.add_parser("softdevice", help="program the softdevice")
softdevice.set_defaults(command="softdevice")
softdevice.add_argument("uicr")
softdevice.add_argument("main")

###############################################################################

jlinkexe = "LD_LIBRARY_PATH={path}:$LD_LIBRARY_PATH {path}/JLinkExe {script}"

def exec_jlinkexe(script, path=os.environ["BUILD_PATH"]):
	try:
		print subprocess.check_output(jlinkexe.format(
						path=os.environ["JLINK_PATH"],
						script=os.path.join(path, script)
						), shell=True)
	except subprocess.CalledProcessError, e:
		print e.output
	return 0

def read_script_file(script):
	return open(os.path.join(os.environ["SCRIPTS_PATH"], script), 'r').read()

def create_tmp_script(name, content):
	with open(os.path.join(os.environ["BUILD_PATH"], name), 'w') as f:
		f.write(content)

###############################################################################

def erase():
	return exec_jlinkexe("erase.jlink", os.environ["SCRIPTS_PATH"])

def flash(program):
	if os.environ["USE_SOFTDEVICE"] == "s110":
		addr = hex(0x00014000)
	else:
		addr = hex(0)

	script_name = "flash.jlink"
	content = read_script_file(script_name).format(program=program, addr=addr)
	create_tmp_script(script_name, content)
	return exec_jlinkexe(script_name)

def softdevice(uicr, main):
	script_name = "softdevice.jlink"
	content = read_script_file(script_name).format(uicr=uicr, main=main)
	create_tmp_script(script_name, content)
	return exec_jlinkexe(script_name)

###############################################################################

args = parser.parse_args().__dict__
command = args.pop("command")

status = globals()[command](**args)
sys.exit(status)
