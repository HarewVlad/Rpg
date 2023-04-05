#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <timeapi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <assert.h>
#include <memory>
#include <commdlg.h>

// Asio
// #include <asio.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_ds.h>

#include "vector.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

using namespace DirectX;
// using asio::ip::tcp;

#include "utils.h"
#include "constants.h"
#include "config.h"
#include "window.h"
#include "directx.h"
#include "interface.h"
#include "animationee.h"
#include "map_editor.h"
#include "mesh.h"
#include "camera.h"
#include "core.h"

#include "test.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "comdlg32.lib")

// TODO: Create map-creator

// https://rafaelmatos.itch.io/epic-rpg-world-pack-old-prison-asset-tileset
// https://itch.io/profile/rgsdev - Good character animations
// https://narehop.itch.io/top-down-rpg - Cool top-down tiles!

// Tile palette:
// 3. Create new sub window :: DONE
// 4. Load texture :: DONE
// 5. Display texture in sub window :: DONE
// 7. Display grid on top of an image :: DONE
// 9. Make zoom in and zoom under "Options" menu :: DONE
// 10. Ability to load multiple of images to select from :: DONE
// 14. Draw grid with option to resize it (16, 32, 64) :: DONE
// 15. Create "RenderGrid" methods cause the same code used twice :: DONE 
// 16. Ability to move around the grid :: DONE
// 17. Rect around grid to be more pretty :: DONE
// 18. Ability to use mouse on image (InvisibleButton) :: DONE
// 19. Ability to press button and get current row and column :: DONE
// 20. Save only 1 current pressed tile for an image (for now) :: DONE
// 21. Highlight selected tile :: DONE
// 22. Fix issue when loading multiple images grid scales to the biggest image :: DONE
// 24. Fix issue with grid not showing correctly when zoom value is too hight :: DONE
// 25. Get click position on the map :: DONE
// 25.1. Create ImGUi::InvisibleButton all around the grid :: DONE
// 26. Save clicked tile :: DONE
// 27. Draw selected tile on the map :: DONE
// 28. Optimize :: DONE
// 29. Issue with grid step: When save tile it saves previous grid step. Need to check good it or not :: DONE
// 30. Ability to store multiple map elements :: DONE
// 31. Checkbox to show / hide grid :: DONE
// 32. Ability to remove one / multiple tiles using right mouse button :: DONE
// 32.1. Fix issue in remove order of objects and bug that they don't remove because of order issue :: DONE
// 33. When switch between images in tile palette, remove active tile from previous image :: DONE
// 34. Store only 1 tile in map, not many because of the array structure :: DONE
// 34.1. Optimize memory layout of data in map creator :: DONE
// 35. Ability to select multiple tiles at once from an image :: DONE
// 36. Draw preview of tiles on map :: DONE
// 37. Make them half transparent :: DONE
// 38. Undo tiles by pressing CTRL + Z :: DONE
// 39. Remove tiles buy pressing right mouse button :: DONE
// 40. Place tiles on the map :: DONE
// 41. Save map :: DONE
// 42. Load map :: DONE
// 43. Fix issue when saving the map and placing tile that is beyond map constraints it is still displayed :: DONE
// 44. Fix positioning of tiles on the map :: DONE
// 44.1 Select only 1 tile when pressing mouse :: DONE
// 44.2 Select multiple tiles when holding mouse :: DONE
// 45. Fix issue with loading map that used multiple images :: DONE
// 46. Rewrite map editor to make it store multiple images at the same tile :: DONE
// 48. Grid change step equal to 8 :: DONE
// 49. Fix issue with tiles selectin incorrectly when zoom is adjusted :: DONE
// 50. Fix issue with double click is required to select tiles when zoom in adjusted
// 51. Remove repating image paths (Hight memory usage when loading map) :: DONE
// 53. Save map via GetSaveFileNameA function :: DONE
// 54. Allocate strings using stb array :: DONE
// 56. Separate images for tile palette and animationee :: DONE
// 57. Replace hash maps with single byte stream and get data using template :: DONE
// 58. Create animationee :: DONE
// 58.1. Create data structure :: DONE
// 58.2. Select images from drive and group them together :: DONE
// 59. Rewrite map editor's serialization as byte stream instead of string stream and use C functions :: DONE
// 60. More flexible grid step selection :: DONE
// 61. Create character checkbox to add character to the map :: DONE
// 62. Small refactor of image initialization :: DONE
// 63. New interface for selecting animation action and grid step :: DONE
// 64. Inside animationee make not list of images, but animation using this images :: DONE
// 65. Make sure that every image that is loaded is stored in core :: DONE 
// 66. Add animation to character on the map :: DONE
// 67. Fix issue when adding multiple animations in animationee you can't use combo box for second one :: DONE
// 69. Ability to select same images in animationee :: DONE
// 69. Ability to "Play" the map :: DONE
// 69.1. Create shaders :: DONE
// 69.2. Create input layout :: DONE
// 69.3. Create constant buffer :: DONE
// 69.4. Create meshes :: DONE
// 69.5. Draw test mesh when click "Test map" :: DONE
// 69.6. Draw meshes correponding to tiles in map editor :: DONE
// 69.7. Fix issue with wrong order? of map objects :: DONE
// 69.8. Make map in map editor and map in map test match :: DONE
// 69.9. Draw character and it's animation :: DONE
// 69.10. Character movement? 
// 70. Save/Load map size :: DONE
// 71. Fix issue when zooming map, can't place tiles outside of some region :: DONE
// 72. Use array instead of hash_map for character animations cause we know max size :: DONE
// 73. Save grid step when saving a map :: DONE

// N - 9. Use 2 meshes for character and map to not to update mesh everytime for character
//        Or think about something else
// N - 8. Use one big vertex and index buffer for everything?
// N - 6. Merge interface modules (TilePalette, MapEditor ...)?
// N - 5. String builder for better strings and serialization (Jonothan Blow)
// N - 2. Display not full path, but only filename in tab item
// N - 1. Chest spawner
// N. Layers in the map creation?
// BONUS: Fix zoom + scrolling mouse position issue :: LATER