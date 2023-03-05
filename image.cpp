void Image::Initialize(Directx *directx, const char *filename){
  texture = directx->CreateTexture(filename, &size.x, &size.y);
}

// void Image::Uninitialize() {
//   if (texture) texture->Release();
// }