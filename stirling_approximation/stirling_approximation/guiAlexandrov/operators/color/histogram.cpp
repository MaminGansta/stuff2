
template <typename Image_type>
struct Histogram
{
	const Image_type* image = NULL;
	float brightness[256];
	float red[256];
	float green[256];
	float blue[256];

	uint32_t cbrightness[256];
	uint32_t cred[256];
	uint32_t cgreen[256];
	uint32_t cblue[256];


	Histogram() {}

	Histogram(const Image_type& image)
	{
		set_img(image);
		update_info();
	}

	void set_img(const Image_type& img)
	{
		image = &img;
	}

	void update_info()
	{
		memset(cbrightness, 0, 256 * sizeof(uint32_t));
		memset(cred, 0, 256 * sizeof(uint32_t));
		memset(cgreen, 0, 256 * sizeof(uint32_t));
		memset(cblue, 0, 256 * sizeof(uint32_t));

		int size = image->width * image->height;
		for (int i = 0; i < size; i++)
		{
			Color color = image->data[i];
			uint8_t br = (color.r + color.g + color.b) / 3;
			cbrightness[br]++;
			cred[color.r]++;
			cgreen[color.g]++;
			cblue[color.b]++;
		}

		for (int i = 0; i < 256; i++)
			brightness[i] = float(cbrightness[i]) / size;

		for (int i = 0; i < 256; i++)
			red[i] = float(cred[i]) / size;

		for (int i = 0; i < 256; i++)
			green[i] = float(cgreen[i]) / size;

		for (int i = 0; i < 256; i++)
			blue[i] = float(cblue[i]) / size;
	}

};

