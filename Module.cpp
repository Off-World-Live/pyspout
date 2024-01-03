#include "pybind11/include/pybind11/pybind11.h"
#include "pybind11/include/pybind11/numpy.h"
#include "Spout2/SPOUTSDK/SpoutGL/Spout.h"

namespace py = pybind11;

class SpoutSender {
public:
	SpoutSender(std::string name, int width, int height, GLenum glFormat = GL_RGB)
	{
		if (name.size() > 256) {
			// only save the first 255 chars
			py::print("Name over 255 characters, truncating...");
			Name = name.substr(0, 255);
		}
		else {
			Name = name;
		}
		Width = width;
		Height = height;
		ChannelsPerPixel = glFormat == GL_RGBA ? 4 : 3;
		GlFormat = glFormat;
		PixelArray.resize(width * height * ChannelsPerPixel);
		SpoutRef = std::make_unique<Spout>();
	};
	~SpoutSender()
	{
		if (!SpoutRef) return;
		SpoutRef->ReleaseSender();
	}

public:
	bool SendTexture(GLuint TextureID, GLuint TextureTarget)
	{
		if (!SpoutRef) return false;
		TryCreateSender();
		return SpoutRef->SendTexture(TextureID, TextureTarget, Width, Height, false, 0);
	}

	bool SendImage(py::array_t<int> pixels, bool bInvert = false)
	{
		if (!SpoutRef) return false;
		TryCreateSender();
		auto buf = pixels.request();
		void* ptr = buf.ptr;
		for (int i = 0; i < (Width * Height * ChannelsPerPixel); i++) {
			uint8_t* val = static_cast<uint8_t*>(ptr) + i * 4;
			PixelArray[i] = *val;
		}
		return SpoutRef->SendImage(static_cast<unsigned char*>(PixelArray.data()), Width, Height, GlFormat, bInvert);
	}

	void Release()
	{
		if (!SpoutRef) return;
		if (!SenderCreated) return;
		SpoutRef->ReleaseSender();
		SenderCreated = false;
	}
public:
	static SpoutSender* Create(std::string name, int width, int height, GLenum glFormat = GL_RGB)
	{
		return new SpoutSender(name, width, height, glFormat);
	}

private:
	bool TryCreateSender()
	{
		if (!SenderCreated)
		{
			if (!SpoutRef->CreateSender(Name.data(), Width, Height, 0))
			{
				py::print("Failed to create spout sender");
				return false;
			};
			SenderCreated = true;
		}
		return true;
	}
	std::string Name;
	bool SenderCreated = false;
	unsigned int Width;
	unsigned int Height;
	int ChannelsPerPixel = 3;
	GLenum GlFormat;
	std::unique_ptr<Spout> SpoutRef;
	std::vector<uint8_t> PixelArray;
};


class SpoutReceiver {
public:
	SpoutReceiver(std::string name, unsigned int w, unsigned int h)
	{
		if (name.size() > 256) {
			// only save the first 255 chars
			py::print("Name over 255 characters, truncating...");
			Name.resize(255);
			std::copy(name.begin(), name.begin() + 255, Name.begin());
		}
		else {
			Name.resize(name.size());
			std::copy(name.begin(), name.end(), Name.begin());
		}
		SpoutRef = std::make_unique<Spout>();
		Width = w;
		Height = h;
	};

	~SpoutReceiver()
	{
		if (!SpoutRef) return;
		SpoutRef->ReleaseReceiver();
	}

public:
	
	bool receiveTexture(GLuint TextureID, GLuint TextureTarget)
	{
		if (!SpoutRef) return false;
		TryCreateReceiver();
		return SpoutRef->ReceiveTexture(TextureID, TextureTarget);
	}

	void Release()
	{
		if (!SpoutRef) return;
		if (!ReceiverCreated) return;
		SpoutRef->ReleaseReceiver();
		ReceiverCreated = false;
	}

public:
	static SpoutReceiver* Create(std::string name, unsigned int w, unsigned int h)
	{
		return new SpoutReceiver(name, w, h);
	}

private:
	bool TryCreateReceiver()
	{
		if (!ReceiverCreated)
		{
			if (!SpoutRef->CreateReceiver(Name.data(), Width, Height))
			{
				py::print("Failed to create spout receiver");
				return false;
			};
			ReceiverCreated = true;
		}
		return true;
	}

	std::vector<char> Name;
	bool ReceiverCreated = false;
	unsigned int Width;
	unsigned int Height;
	GLenum GlFormat;
	std::unique_ptr<Spout> SpoutRef;
};

PYBIND11_MODULE(PySpout, s) {
	py::class_<SpoutSender>(s, "SpoutSender")
		.def(py::init(&SpoutSender::Create))
		.def("send_texture", &SpoutSender::SendTexture)
		.def("send_image", &SpoutSender::SendImage)
		.def("release", &SpoutSender::Release);


	py::class_<SpoutReceiver>(s, "SpoutReceiver")
		.def(py::init(&SpoutReceiver::Create))
		.def("receive_texture", &SpoutReceiver::receiveTexture)
		.def("release", &SpoutReceiver::Release);
};