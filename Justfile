
build:
	make -C gvrinput -j $(nproc)
	make -C launcher -j $(nproc)

clean:
	make -C gvrinput clean
	make -C launcher clean

# makes a package assemblage.
package:
	mkdir pkg
	just build
	cp dist/* pkg/
	cp LICENSE pkg/
	cp gvrinput/GVRInputRaw.dll pkg/
	cp launcher/invlaunch.exe pkg/
	cd pkg; 7z a ../InvasionPC.7z *; cd ..
	rm -rf pkg
