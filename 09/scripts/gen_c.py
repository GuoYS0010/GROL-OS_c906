import sys, getopt

if __name__ == "__main__":
    input_filename = "os.bin"
    bindir = "bin/"
    output_filename = "pat_c.h"
    filedir = "build/"

    print(filedir+input_filename, "is generating")
    file_in = open(bindir+input_filename, 'rb')
    file_out = open(filedir+output_filename, 'w')
    file_out.write("#include \"ram_load.h\"\n\r\n\r\n\r")
    file_out.write("void ram_load(){\n\r")
    offset = 0
    code = file_in.read(4)
    while code:
        data = hex(int.from_bytes(code, byteorder='little'))
        file_out.write("\tXil_Out32(" + hex(offset) + ", " + data + ");\n" )
        offset += 4
        code = file_in.read(4)
    
    # while  data:
    #     value = hex(int.from_bytes(data, byteorder='little'))
    #     file_out.write("\tXil_Out32(" + hex(offset) + ", " + value + ");\n" )
    #     offset += 4
    #     file_in.seek(base_addr + offset)
    #     data = file_in.read(4)

    file_out.write("\n}")

        
    




    file_in.close()
    file_out.close()
