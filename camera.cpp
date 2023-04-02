XMMATRIX Camera::GetView() {
  return XMMatrixLookAtRH({position.x, position.y, -1},
                          {position.x, position.y, 1}, {0, 1, 0}); 
}