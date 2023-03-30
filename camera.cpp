XMMATRIX Camera::GetView() {
  XMFLOAT3 p = {position.x, position.y, -1};
  XMFLOAT3 t = {position.x, position.y, 1};
  return XMMatrixLookAtLH(XMLoadFloat3(&p), XMLoadFloat3(&t), XMVECTOR{0, 1, 0}); 
}