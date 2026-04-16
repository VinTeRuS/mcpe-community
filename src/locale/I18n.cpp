#include "I18n.h"
#include <sstream>
#include "../AppPlatform.h"
#include "../util/StringUtils.h"
#include "../world/level/tile/Tile.h"
#include "../world/item/ItemInstance.h"
#include "../platform/log.h"
#include <ctype.h>

I18n::Map I18n::_strings;

void I18n::loadLanguage( AppPlatform* platform, const std::string& languageCode )
{
	_strings.clear();
	fillTranslations(platform, "lang/en_US.lang", true);

	if (languageCode != "en_US")
		fillTranslations(platform, "lang/" + languageCode + ".lang", true);
}

bool I18n::get( const std::string& id, std::string& out ) {
	Map::const_iterator cit = _strings.find(id);
	if (cit != _strings.end()) {
		out = cit->second;
		return true;
	}
	return false;
}

std::string I18n::get( const std::string& id )
{
	Map::const_iterator cit = _strings.find(id);
	if (cit != _strings.end())
		return cit->second;

	return id + '<';//lang.getElement(id);
}

void I18n::fillTranslations( AppPlatform* platform, const std::string& filename, bool overwrite )
{
	BinaryBlob blob = platform->readAssetFile(filename);
	if (!blob.data || blob.size <= 0)
		return;

	std::string data((const char*)blob.data, blob.size);
	std::stringstream fin(data, std::ios_base::in);

	std::string line;
	while( std::getline(fin, line) ) {
		int spos = line.find('=');
		if (spos == std::string::npos)
			continue;

		std::string key   = Util::stringTrim(line.substr(0, spos));
		Map::const_iterator cit = _strings.find(key);
		if (!overwrite && cit != _strings.end())
			continue;

		std::string value = Util::stringTrim(line.substr(spos + 1));
		_strings.insert( std::make_pair(key, value ) );
	}

	delete[] blob.data;
}

std::string I18n::getDescriptionString( const ItemInstance& item )
{
	const std::string desc = item.getDescriptionId();
	std::string trans;

	std::string descLower = desc;
	for (unsigned int i = 0; i < descLower.length(); ++i)
		descLower[i] = ::tolower(descLower[i]);

	if (item.id == Tile::cloth->id) {
		if (get(descLower + ".name", trans))
			return trans;
		return get("tile.cloth.name");
	} else if (item.id == Tile::fenceGate->id) {
		return get("tile.fence.name");
	} else if (item.id == Tile::stoneSlabHalf->id) {
		if (get(descLower + ".name", trans))
			return trans;
		return get("tile.stoneslab.stone.name");
	}

	std::string prefix = "item.";
	if (descLower.length() >= 5 && descLower.substr(0, 5) == "tile.")
		prefix = "tile.";

	std::string key = descLower;
	if (prefix == "tile." || prefix == "item.")
		key = descLower.substr(prefix.length());

	if (get(prefix + key + ".name", trans))
		return trans;

	return descLower + " : couldn't find name";
}

std::string I18n::getItemDescription( const std::string& descriptionId )
{
	std::string id = descriptionId;
	for (unsigned int i = 0; i < id.length(); ++i)
		id[i] = ::tolower(id[i]);

	if (id.substr(0, 5) == "item.")
		id = id.substr(5);
	else if (id.substr(0, 5) == "tile.")
		id = id.substr(5);

	return get("desc." + id);
}
