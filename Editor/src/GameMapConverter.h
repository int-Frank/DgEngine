#ifndef GAMEMAPCONVERTER_H
#define GAMEMAPCONVERTER_H

#include <string>

#include "MapData.h"
#include "Types.h"

class GameMapConverter
{
public:

  GameMapConverter();
  ~GameMapConverter();

  void SetInputFiles(std::string const & mapHeader,
                     std::string const & gameData);

  void SetOutputFile(std::string const & mapHeader);

  void Convert();

private:

  GameMapConverter(GameMapConverter const &);
  GameMapConverter & operator=(GameMapConverter const &);

private:

  class PIMPL;
  PIMPL * m_pimpl;
};

#endif