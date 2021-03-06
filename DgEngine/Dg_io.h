/*!
* @file Dg_io.h
*
* @author Frank Hart
* @date 2/01/2014
*
* This file contains definitions and functions to assist with reading and 
* writing to and from different media.
*/

#ifndef DG_IO_H
#define DG_IO_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "DgTypes.h"
#include "pugixml.hpp"

/** \addtogroup utility_io
*  @{
*/
typedef std::ostream		DgWriter;
typedef std::istream		DgReader;
typedef std::stringstream	DgString;
typedef std::ifstream		DgFileReader;
typedef std::ofstream		DgFileWriter;
/** @}*/

//--------------------------------------------------------------------------------
//	@	IgnoreComments()
//--------------------------------------------------------------------------------
/*!
 * @ingroup utility_io
 *
 * @brief Ignore comments when reading from an instream.
 *
 * @param[in] in The instream.
 * @param[in] c The comment initiation character.
 *
 * @return Returns the instream to support chaining.
 */
DgReader& IgnoreComments(DgReader& in, char c = '#');


//--------------------------------------------------------------------------------
//	@	LoadFile()
//--------------------------------------------------------------------------------
/*!
 * @ingroup utility_io
 *
 * @brief Loads a file into an object.
 *
 * The file will be read as a DgFileReader and loaded into the object with the
 * '>>' operator.
 *
 * @param[in] file The name of the file to load (path + name + extension).
 * @param[out] dest The object to load into.
 *
 * @return True if file was successfully loaded.
 */
template<class T>
bool LoadFile(const std::string& file, T& dest)
{
	//Open file
	DgFileReader file_in(file);

	//Make sure it opened
	if (!file_in)
	{
        std::cerr << "@LoadFile() -> Failed to open file: " << file << std::endl;
		return false;
	}

	//Read to object
	file_in >> dest;

	return true;
}	//End: LoadFile()


//--------------------------------------------------------------------------------
//	@	LoadXMLFile()
//--------------------------------------------------------------------------------
/*!
 * @ingroup utility_io
 *
 * @brief Loads an XML file into an object.
 *
 * The file will be read as an xml file and loaded into the object with the
 * '>>' operator.
 *
 * @param[in] file The name of the file to load (path + name + extension).
 * @param[out] dest The object to load into.
 *
 * @return True if file was successfully loaded.
 */
template<class T>
bool LoadXMLFile(const std::string& file, T& dest)
{
	//Open file
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(file.c_str());

	//Make sure it opened
	if (!result)
	{
        std::cerr << "@DgUtility::LoadXMLFile() -> Failed to open file: " << file << std::endl;
		return false;
	}

	//Get the root node
	pugi::xml_node rootNode = doc.document_element();

	rootNode >> dest;

	return true;
}	//End: LoadXMLFile()


//--------------------------------------------------------------------------------
//	@	ReadTo()
//--------------------------------------------------------------------------------
/*!
*/
template<class T>
bool ReadTo(const std::string& tag, T& dest)
{
    std::istringstream iss(tag);
    return !(iss >> dest).fail();

}	//End: ReadTo()


//--------------------------------------------------------------------------------
//	@	StringToNumber()
//--------------------------------------------------------------------------------
/*!
 * @ingroup utility_io
 *
 * @brief Converts a string to a number.
 *
 * @param[out] t Output number.
 * @param[in] s Input string.
 * @param[in] f The base fo the number to read. (eg std::dec)
 *
 * @return True if a number was loaded into the output.
 */
template <class T>
bool StringToNumber(T& t, 
					const std::string& s, 
					std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();

}	//End: StringToNumber()


//--------------------------------------------------------------------------------
//	@	ToBool()
//--------------------------------------------------------------------------------
/*!
 * @ingroup utility_io
 *
 * @brief Converts a string to a boolean.
 *
 * A "false" (any capitalization), "0" or empty string will return false. 
 * All else is true.
 *
 * @param[in] str The input string.
 *
 * @return The boolean.
 */
inline bool ToBool(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    
	return !(str == "false" || str == "0" || str == "");

}	//End: ToBool()

#endif