# fbgl
Simple graphics library for linux framebuffer on the RPI

Framebuffer Graphics Library. (fbgl)

Basic shapes like circle, box, text and line.

This uses the freetype font library.

	sudo apt install -y libfreetype-dev libpng-dev

Also uses my utils library.

	git clone https://github.com/tigerkelly/utils.git

Place the utils library at the same level as the fbgl code.

The example directory contains a simple example.

Using the fbset command check to see what the color bits are.
The monitor I am using shows as 
	mode "1024x600"
		geometry 1024 600 1024 600 32
		timings 0 0 0 0 0 0 0
		rgba 8/16,8/8,8/0,8/24
	endmode

The geometry line contains the bits per pixel (32).
If it does not say 32 bits then use the fbset command like.
	fbset -g 1024 600 1024 600 32

If this does not set the bits per pixel to 32 then you maybe need to remove or comment out the 
	dtoverlay=vc4-kms-v3d

In the /boot/firmware/config.txt file. Reboot and try fbset -g command again.
