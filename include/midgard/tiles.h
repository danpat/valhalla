
#ifndef VALHALLA_MIDGARD_TILES_H_
#define VALHALLA_MIDGARD_TILES_H_

#include <list>
#include <map>

#include "midgard/aabb2.h"

namespace valhalla{
namespace midgard{

/**
 * A class that provides a uniform (square) tiling system for a specified
 * bounding box (either in x,y or f,lng) and tile size.
 * A unique tile ID is assigned for each tile based on the following rules:
 *    Tile numbers start at 0 at the min y, x (lower left)
 *    Tile numbers increase by column (x,longitude) then by row (y,latitude)
 *    Tile numbers increase along each row by increasing x,longitude.
 * Contains methods for converting x,y or lat,lng into tile ID and
 * vice-versa.  Methods for relative tiles (using row and column offsets).
 * are also provided.
 * @author  David W. Nesbitt
 */
class Tiles {
 public:
  /**
   * Constructor.  A bounding box and tile size is specified.
   * Sets class data members and precalculates the number of rows and columns
   * based on the bounding box and tile size.
   * @param   bounds    Bounding box
   * @param   tilesize  Tile size
   */
  Tiles(const AABB2& bounds, const float tilesize);

  /**
   * Destructor.
   */
  virtual ~Tiles();

  /**
  * Get the tile size.
  * @return Tile size.
  */
  float TileSize() const;

  /**
   * Returns the bounding box of the tiling system
   * @return Bounding box.
   */
  AABB2 TileBounds() const;

  /**
   * Gets the "row" based on y.
   * @param   y   y coordinate
   * @return  Returns the tile row. Returns -1 if outside the
   *          tile system bounds.
   */
  int Row(const float y) const;

  /**
   * Gets the "column" based on x.
   * @param   x   x coordinate
   * @return  Returns the tile column. Returns -1 if outside the
   *          tile system bounds.
   */
  int Col(const float x) const;

  /**
   * Converts the center of a bounding box to a tile ID.
   * @param   c   Center point.
   * @return  Returns the tile ID. If the latitude, longitude is outside
   *          the extent, an error (-1) is returned.
   */
  int TileId(const Point2& c);

  /**
   * Converts x,y to a tile ID.
   * @param   y   x (or lng)
   * @param   x   y (or lat)
   * @return  Returns the tile ID. -1 (errors( is returned if the x,y is
   *          outside the bounding box of the tiles.
   */
  int TileId(const float y, const float x);

  /**
   * Gets the tile ID given the row ID and column ID.
   */
  int TileId(const int col, const int row);

  /**
   * Get the base x,y of a specified tile.
   * @param   tileid   Tile ID.
   * @return  The base x,y of the specified tile.
   */
  Point2 Base(const int tileid) const;

  /**
   * Gets the y,x extent of the specified tile.
   * @param   tileid   Tile ID.
   * @return  The latitude, longitude extent of the specified tile.
   */
  AABB2 TileBounds(const int tileid) const;

  /**
   * Gets the y,x extent of the tile with specified row, column.
   * @param   col   Tile column.
   * @param   row   Tile row.
   * @return  The latitude, longitude extent of the specified tile.
   */
  AABB2 TileBounds(const int col, const int row) const;

  /**
   * Gets the center of the specified tile.
   * @param   tileid   Tile ID.
   * @return  The center x,y of the specified tile.
   */
  Point2 Center(const int tileid) const;

  /**
   * Returns the new tile given a previous tile and a row, column offset.
   * @param   initial_tile      ID of the tile to offset from.
   * @param   delta_rows    Number of rows to offset (can be negative).
   * @param   delta_cols    Number of columns to offset (can be negative).
   * @return  Tile ID of the new tile.
   */
  int GetRelativeTileId(const int initial_tile, const int delta_rows,
                        const int delta_cols) const;

   /**
    * Returns the tile offsets (row,column) between the previous tile ID and
    * a new tileid.  The offsets are returned through arguments (references).
    * Offsets can be positive or negative or 0.
    * @param   initial_tileid     Original tile.
    * @param   newtileid      Tile to which relative offset is desired.
    * @param   delta_rows    Return: Relative number of rows.
    * @param   delta_cols    Return: Relative number of columns.
    */
   void TileOffsets(const int initial_tileid, const int newtileid,
                       int& delta_rows, int& delta_cols) const;

  /**
   * Get the number of tiles in the extent.
   * @return  Number of tiles.
   */
  unsigned int TileCount() const;

  /**
   * Gets the neighboring tileid to the right/east.
   */
  int RightNeighbor(const int tileid) const;

  /**
   * Gets the neighboring tileid - west.
   */
  int LeftNeighbor(const int tileid) const;

  /**
   * Gets the neighboring tileid - north.
   */
  int TopNeighbor(const int tileid) const;

   /**
    * Gets the neighboring tileid - south.
    */
   int BottomNeighbor(const int tileid) const;

  /**
   * Gets the list of tiles that lie within the specified bounding box.
   * The method finds the center tile and spirals out by finding neighbors
   * and recursively checking if tile is inside and checking/adding
   * neighboring tiles
   * @param  boundingbox  Bounding box
   * @param  maxTiles  Maximum number of tiles to find.
   */
  const std::vector<int>& TileList(const AABB2& boundingbox,
               const unsigned int maxtiles = 4096);

 protected:
  // Bounding box of the tiling system.
  AABB2 tilebounds_;

  // Tile size.  Tiles are square (equal y and x size).
  float tilesize_;

  // Number of rows ( y or latitude)
  int nrows_;

  // Number of longitude (x or longitude).
  int ncolumns_;

  // Tile list being constructed
  std::vector<int> tilelist_;

  // List of tiles to check if in view. Use a list: push new entries on the
  // back and pop off the front. The tile search tends to spiral out from
  // the center.
  std::list<int> checklist_;

  // Visited tiles
  std::map<int, int> visitedtiles_;

  // Default constructor (private).  Forces use of the bounding box
  Tiles();

  // This function checks neighboring tiles. It adds these tiles to the
  // end of the CheckList if they are not already in there.
  void addNeighbors(const int tileid);

  // Returns the next tile from the check list that is inside the bounding
  // box. Adds its neighbors to the check list.
  // Returns the tileId or -1 if no more tiles are inside the bounding box.
  int NextTile(const AABB2& boundingbox);

  // Convenience method to check if the tile has already been considered
  // (added to tile list or check list)
  bool InList(const int id);
};

}
}

#endif  // VALHALLA_MIDGARD_TILES_H_
