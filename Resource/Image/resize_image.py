from PIL import Image
def resize_image(image_path, output_path):
    image = Image.open(image_path)
    width, height = image.size
    scale = 100/height
    new_width = int(width * scale)
    resized_image = image.resize((new_width, 100), Image.Resampling.LANCZOS)
    resized_image.save(output_path)

resize_image('logo_0.jpg', 'logo.jpeg')
