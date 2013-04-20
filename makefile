LIB = Gdiplus Gdi32 ole32 oleaut32 uuid winmm msimg32 glaux glu32 opengl32 kernel32

out:
	gcc $(SRC) $@.c -I$(INCLUDE_DIR) $(LIB:%=-l%) -DWINVER=0x0501 -o $@ 
