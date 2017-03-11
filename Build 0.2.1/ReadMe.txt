Who: Anthony RamosLebron Creation

What: A Windows Console App using the Windows Imaging Component (WIC) API to Load/Save/Edit images in this app
a waterMark is being placed on a image using two images that the user inputed.

Support Images: .jpg, .png, .bmp, .tif, .gif

Supported System: Windows 8.1 and up.

Known Bugs: Entire width of WaterMark image won't show up*Fixed*; if Opacity is set too low for certain images some pixels won't show.

Source Code/Github can be found at: https://github.com/Tonykidv2/Windows-Imaging-Component-WaterMarking


How-To: 
1.)Open CMD 
2.)enter .exe name then the two images to to process a waterMark then press Enter
	example: C:\\FakePak\\ImagaWaterMarking.exe C:\\FakePath\\Image1.png C:\\FakePath\\Image2.tiff *Press Enter*
3.)Enter Opacity Level between 0-100
4.)Enter Scale of the second Image between 0-100 *if the image is bigger than the first the waterMark image is scaled to the first image
5.)Decide if you want to ignore a color if the watermark image doesnt have transparency place the RGB values between 0-255
6.)Decide if you want to center the image if the watermark image is smaller than the base image
7.)Press Enter *program will do the rest*
8.)Look for Output.xxx image file which is in the same directory as the exe file.