# PySpout

A simple [Spout2](https://github.com/leadedge/spout2) implementation for sending opengl textures and raw pixel data to spout

## Usage with Raw Pixel Data

1. Add the `Spout.dll` file and the `PySpout.pyd` files to your project directory.

2. Import the `SpoutSender` from `PySpout`

3. Create your sender with name and dimensions

4. Call `sender.send_image(frame)` where `frame` is a numpy array

```python
# import module
from PySpout import SpoutSender
from OpenGL.GL import GL_RGB


# Create the sender
sender = SpoutSender("MySpoutName", 640, 480, GL_RGB)

while True:
	# get the data as a integer numpy ndarray
	data = get_my_data()

	# try and send the data to spout (use GL_RGBA if your data includes an alpha channel)
	if not sender.send_image(data)

# Finally, when exiting the program, ensure you release the sender 

sender.release()
```

## Usage with OpenGL

Follow steps 1. to 3. above, then:

1. Create a `GL_TEXTURE_2D` with a `texture id`
2. Call  `sender.send_texture(texture_id, GL_TEXTURE_2D)`

## Development / Building yourself

1. Clone this repo recursively

```
git clone --recursive git@github.com:off-world-live/pyspout.git
```
2. Install python with the official windows installer (not via Microsoft Store), selecting `Add to Path` and `Include Debugging Symbols`

### Using a different version of python to 3.10

This configuration assumes python has been installed to `C:\Users\<yourname>\AppData\Local\Programs\Python310`. If you've installed
a different python version, you will need to update the following configurations:

3. Open [PySpout.sln](./PySpout.sln) with Visual Studio, right click on the `PySpout` project and open `Properties`

4. Select `All Configurations` from the dropdown

5. Open the `C/C++` section and click on `General`

6. Edit `Additional Include Directories` and add `<your python install path>\include`

7. Open the `Linker` section and click on `General`

8. Edit `Additional Library Directories` and add `<your python install path>\libs`

9. Click Apply.

### Fixing Spout Post-Build

By default the spout build config will try to copy to a non-existent `Binaries/x64` folder.
Fix this by:

1. Clicking on `SpoutSDK` in the Visual Studio Solutions Explorer, and choosing `Properties`
2. Choose `BuildEvents->Post-Build Event`
3. Delete the `Command Line` option
4. Click `Apply`


Now you should be able to build :)

## License

This implementation is copyright Campbell Morgan, Off World Live Limited
but is released open source under the [MIT License](./LICENSE)
