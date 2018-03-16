/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "xeen/item.h"
#include "xeen/resources.h"
#include "xeen/xeen.h"
#include "xeen/dialogs/dialogs_query.h"

namespace Xeen {

XeenItem::XeenItem() {
	clear();
}

void XeenItem::clear() {
	_material = _id = _bonusFlags = 0;
	_frame = 0;
}

void XeenItem::synchronize(Common::Serializer &s) {
	s.syncAsByte(_material);
	s.syncAsByte(_id);
	s.syncAsByte(_bonusFlags);
	s.syncAsByte(_frame);
}

ElementalCategory XeenItem::getElementalCategory() const {
	int idx;
	for (idx = 0; Res.ELEMENTAL_CATEGORIES[idx] < _material; ++idx)
		;

	return (ElementalCategory)idx;
}

AttributeCategory XeenItem::getAttributeCategory() const {
	int m = _material - 59;
	int idx;
	for (idx = 0; Res.ATTRIBUTE_CATEGORIES[idx] < m; ++idx)
		;

	return (AttributeCategory)idx;
}

const char *XeenItem::getItemName(ItemCategory category, uint id) {
	if (id < 82)
		return Res.ITEM_NAMES[category][id];

	switch (category) {
	case CATEGORY_WEAPON:
		return Res.QUEST_ITEM_NAMES[id - 82];

	case CATEGORY_ARMOR:
		return Res.QUEST_ITEM_NAMES[id - 82 + 35];

	case CATEGORY_ACCESSORY:
		return Res.QUEST_ITEM_NAMES[id - 82 + 35 + 14];

	default:
		return Res.QUEST_ITEM_NAMES[id - 82 + 35 + 14 + 11];
	}
}

/*------------------------------------------------------------------------*/

InventoryItems::InventoryItems(Character *character, ItemCategory category):
		_character(character), _category(category) {
	resize(INV_ITEMS_TOTAL);

	_names = Res.ITEM_NAMES[category];
}

void InventoryItems::clear() {
	for (uint idx = 0; idx < size(); ++idx)
		operator[](idx).clear();
}

bool InventoryItems::passRestrictions(int itemId, bool suppressError) const {
	CharacterClass charClass = _character->_class;

	switch (charClass) {
	case CLASS_KNIGHT:
	case CLASS_PALADIN:
		return true;

	case CLASS_ARCHER:
	case CLASS_CLERIC:
	case CLASS_SORCERER:
	case CLASS_ROBBER:
	case CLASS_NINJA:
	case CLASS_BARBARIAN:
	case CLASS_DRUID:
	case CLASS_RANGER: {
		if (!(Res.ITEM_RESTRICTIONS[itemId + Res.RESTRICTION_OFFSETS[_category]] &
			(1 << (charClass - CLASS_ARCHER))))
			return true;
		break;
	}

	default:
		break;
	}

	Common::String name = _names[itemId];
	if (!suppressError) {
		Common::String msg = Common::String::format(Res.NOT_PROFICIENT,
			Res.CLASS_NAMES[charClass], name.c_str());
		ErrorScroll::show(Party::_vm, msg, WT_FREEZE_WAIT);
	}

	return false;
}

Common::String InventoryItems::getName(int itemIndex) {
	int id = operator[](itemIndex)._id;
	return _names[id];
}

Common::String InventoryItems::getIdentifiedDetails(int itemIndex) {
	XeenItem &item = operator[](itemIndex);

	Common::String classes;
	for (int charClass = CLASS_KNIGHT; charClass <= CLASS_RANGER; ++charClass) {
		if (passRestrictions(charClass, true)) {
			const char *const name = Res.CLASS_NAMES[charClass];
			classes += name[0];
			classes += name[1];
			classes += " ";
		}
	}
	if (classes.size() == 30)
		classes = Res.ALL;

	return getAttributes(item, classes);
}

bool InventoryItems::discardItem(int itemIndex) {
	XeenItem &item = operator[](itemIndex);
	XeenEngine *vm = Party::_vm;

	if (item._bonusFlags & ITEMFLAG_CURSED) {
		ErrorScroll::show(vm, Res.CANNOT_DISCARD_CURSED_ITEM);
	} else {
		Common::String itemDesc = getFullDescription(itemIndex, 4);
		Common::String msg = Common::String::format(Res.PERMANENTLY_DISCARD, itemDesc.c_str());

		if (Confirm::show(vm, msg)) {
			operator[](itemIndex).clear();
			sort();

			return true;
		}
	}

	return true;
}

void InventoryItems::sort() {
	for (uint idx = 0; idx < size(); ++idx) {
		if (operator[](idx)._id == 0) {
			// Found empty slot
			operator[](idx).clear();

			// Scan through the rest of the list to find any item
			for (uint idx2 = idx + 1; idx2 < size(); ++idx2) {
				if (operator[](idx2)._id) {
					// Found an item, so move it into the blank slot
					operator[](idx) = operator[](idx2);
					operator[](idx2).clear();
					break;
				}
			}
		}
	}
}

void InventoryItems::removeItem(int itemIndex) {
	XeenItem &item = operator[](itemIndex);
	XeenEngine *vm = Party::_vm;

	if (item._bonusFlags & ITEMFLAG_CURSED)
		ErrorScroll::show(vm, Res.CANNOT_REMOVE_CURSED_ITEM);
	else
		item._frame = 0;
}

XeenEngine *InventoryItems::getVm() {
	return Party::_vm;
}

void InventoryItems::equipError(int itemIndex1, ItemCategory category1, int itemIndex2,
		ItemCategory category2) {
	XeenEngine *vm = Party::_vm;

	if (itemIndex1 >= 0) {
		Common::String itemName1 = _character->_items[category1].getName(itemIndex1);
		Common::String itemName2 = _character->_items[category2].getName(itemIndex2);

		MessageDialog::show(vm, Common::String::format(Res.REMOVE_X_TO_EQUIP_Y,
			itemName1.c_str(), itemName2.c_str()));
	} else {
		MessageDialog::show(vm, Common::String::format(Res.EQUIPPED_ALL_YOU_CAN,
			(itemIndex1 == -1) ? Res.RING : Res.MEDAL));
	}
}

void InventoryItems::enchantItem(int itemIndex, int amount) {
	XeenEngine *vm = Party::_vm;
	vm->_sound->playFX(21);
	ErrorScroll::show(vm, Common::String::format(Res.NOT_ENCHANTABLE, Res.SPELL_FAILED));
}

bool InventoryItems::isFull() const {
	assert(size() == INV_ITEMS_TOTAL);
	return operator[](size() - 1)._id != 0;
}

/*------------------------------------------------------------------------*/

void WeaponItems::equipItem(int itemIndex) {
	XeenItem &item = operator[](itemIndex);

	if (item._id <= 17) {
		if (passRestrictions(item._id)) {
			for (uint idx = 0; idx < size(); ++idx) {
				XeenItem &i = operator[](idx);
				if (i._frame == 13 || i._frame == 1) {
					equipError(itemIndex, CATEGORY_WEAPON, idx, CATEGORY_WEAPON);
					return;
				}
			}

			item._frame = 1;
		}
	} else if (item._id >= 30 && item._id <= 33) {
		if (passRestrictions(item._id)) {
			for (uint idx = 0; idx < size(); ++idx) {
				XeenItem &i = operator[](idx);
				if (i._frame == 4) {
					equipError(itemIndex, CATEGORY_WEAPON, idx, CATEGORY_WEAPON);
					return;
				}
			}

			item._frame = 4;
		}
	} else {
		if (passRestrictions(item._id)) {
			for (uint idx = 0; idx < size(); ++idx) {
				XeenItem &i = operator[](idx);
				if (i._frame == 13 || i._frame == 1) {
					equipError(itemIndex, CATEGORY_WEAPON, idx, CATEGORY_WEAPON);
					return;
				}
			}

			for (uint idx = 0; idx < _character->_armor.size(); ++idx) {
				XeenItem &i = _character->_armor[idx];
				if (i._frame == 2) {
					equipError(itemIndex, CATEGORY_WEAPON, idx, CATEGORY_ARMOR);
					return;
				}
			}

			item._frame = 13;
		}
	}
}

Common::String WeaponItems::getFullDescription(int itemIndex, int displayNum) {
	XeenItem &i = operator[](itemIndex);
	Resources &res = *getVm()->_resources;

	return Common::String::format("\f%02u%s%s%s\f%02u%s%s%s", displayNum,
		!i._bonusFlags ? res._maeNames[i._material].c_str() : "",
		(i._bonusFlags & ITEMFLAG_BROKEN) ? Res.ITEM_BROKEN : "",
		(i._bonusFlags & ITEMFLAG_CURSED) ? Res.ITEM_CURSED : "",
		displayNum,
		Res.WEAPON_NAMES[i._id],
		!i._bonusFlags ? "" : Res.BONUS_NAMES[i._bonusFlags & ITEMFLAG_BONUS_MASK],
		(i._bonusFlags & (ITEMFLAG_BROKEN | ITEMFLAG_CURSED)) ||
			!i._bonusFlags ? "\b " : ""
	);
}

void WeaponItems::enchantItem(int itemIndex, int amount) {
	Sound &sound = *getVm()->_sound;
	XeenItem &item = operator[](itemIndex);
	Character tempCharacter;

	if (item._material == 0 && item._bonusFlags == 0 && item._id != 34) {
		tempCharacter.makeItem(amount, 0, 1);
		XeenItem &tempItem = tempCharacter._weapons[0];

		item._material = tempItem._material;
		item._bonusFlags = tempItem._bonusFlags;
		sound.playFX(19);
	} else {
		InventoryItems::enchantItem(itemIndex, amount);
	}
}

Common::String WeaponItems::getAttributes(XeenItem &item, const Common::String &classes) {
	Common::String attrBonus, elemDamage, physDamage, toHit, specialPower;
	attrBonus = elemDamage = physDamage = toHit = specialPower = Res.FIELD_NONE;

	// First calculate physical damage
	int minVal = Res.WEAPON_DAMAGE_BASE[item._id];
	int maxVal = minVal * Res.WEAPON_DAMAGE_MULTIPLIER[item._id];

	if (item._material >= 37 && item._material <= 58) {
		minVal += Res.METAL_DAMAGE[item._material - 37];
		maxVal += Res.METAL_DAMAGE[item._material - 37];
		toHit = Common::String::format("%+d", Res.METAL_DAMAGE_PERCENT[item._material - 37]);
	}

	physDamage = Common::String::format(Res.DAMAGE_X_TO_Y, minVal, maxVal);

	// Next handle elemental/attribute damage
	if (item._material < 37) {
		int damage = Res.ELEMENTAL_DAMAGE[item._material];
		if (damage > 0) {
			ElementalCategory elemCategory = item.getElementalCategory();
			elemDamage = Common::String::format(Res.ELEMENTAL_XY_DAMAGE,
				damage, Res.ELEMENTAL_NAMES[elemCategory]);
		}
	} else if (item._material >= 59) {
		int bonus = Res.ATTRIBUTE_BONUSES[item._material - 59];
		AttributeCategory attrCategory = item.getAttributeCategory();
		attrBonus = Common::String::format(Res.ATTR_XY_BONUS, bonus,
			Res.ATTRIBUTE_NAMES[attrCategory]);
	}

	// Handle weapon effective against
	int effective = item._bonusFlags & ITEMFLAG_BONUS_MASK;
	if (effective) {
		specialPower = Common::String::format(Res.EFFECTIVE_AGAINST,
			Res.EFFECTIVENESS_NAMES[effective]);
	}

	return Common::String::format(Res.ITEM_DETAILS, classes.c_str(),
		toHit.c_str(), physDamage.c_str(), elemDamage.c_str(),
		Res.FIELD_NONE, Res.FIELD_NONE, attrBonus.c_str(), specialPower.c_str()
	);
}

/*------------------------------------------------------------------------*/

void ArmorItems::equipItem(int itemIndex) {
	XeenItem &item = operator[](itemIndex);

	if (item._id <= 7) {
		if (passRestrictions(item._id)) {
			for (uint idx = 0; idx < size(); ++idx) {
				XeenItem &i = operator[](idx);
				if (i._frame == 9) {
					equipError(itemIndex, CATEGORY_ARMOR, idx, CATEGORY_ARMOR);
					return;
				}
			}

			item._frame = 3;
		}
	} else if (item._id == 8) {
		if (passRestrictions(item._id)) {
			for (uint idx = 0; idx < size(); ++idx) {
				XeenItem &i = operator[](idx);
				if (i._frame == 2) {
					equipError(itemIndex, CATEGORY_ARMOR, idx, CATEGORY_ARMOR);
					return;
				}
			}

			for (uint idx = 0; idx < _character->_weapons.size(); ++idx) {
				XeenItem &i = _character->_weapons[idx];
				if (i._frame == 13) {
					equipError(itemIndex, CATEGORY_ARMOR, idx, CATEGORY_WEAPON);
					return;
				}
			}

			item._frame = 2;
		}
	} else if (item._id == 9) {
		for (uint idx = 0; idx < size(); ++idx) {
			XeenItem &i = operator[](idx);
			if (i._frame == 5) {
				equipError(itemIndex, CATEGORY_ARMOR, idx, CATEGORY_ARMOR);
				return;
			}
		}

		item._frame = 5;
	} else if (item._id == 10) {
		for (uint idx = 0; idx < size(); ++idx) {
			XeenItem &i = operator[](idx);
			if (i._frame == 9) {
				equipError(itemIndex, CATEGORY_ARMOR, idx, CATEGORY_ARMOR);
				return;
			}
		}

		item._frame = 9;
	} else if (item._id <= 12) {
		for (uint idx = 0; idx < size(); ++idx) {
			XeenItem &i = operator[](idx);
			if (i._frame == 10) {
				equipError(itemIndex, CATEGORY_ARMOR, idx, CATEGORY_ARMOR);
				return;
			}
		}

		item._frame = 10;
	} else {
		for (uint idx = 0; idx < size(); ++idx) {
			XeenItem &i = operator[](idx);
			if (i._frame == 6) {
				equipError(itemIndex, CATEGORY_ARMOR, idx, CATEGORY_ARMOR);
				return;
			}
		}

		item._frame = 6;
	}
}

Common::String ArmorItems::getFullDescription(int itemIndex, int displayNum) {
	XeenItem &i = operator[](itemIndex);
	Resources &res = *getVm()->_resources;

	return Common::String::format("\f%02u%s%s%s\f%02u%s%s", displayNum,
		!i._bonusFlags ? "" : res._maeNames[i._material].c_str(),
		(i._bonusFlags & ITEMFLAG_BROKEN) ? Res.ITEM_BROKEN : "",
		(i._bonusFlags & ITEMFLAG_CURSED) ? Res.ITEM_CURSED : "",
		displayNum,
		Res.ARMOR_NAMES[i._id],
		(i._bonusFlags & (ITEMFLAG_BROKEN | ITEMFLAG_CURSED)) ||
			!i._bonusFlags ? "\b " : ""
	);
}

void ArmorItems::enchantItem(int itemIndex, int amount) {
	Sound &sound = *getVm()->_sound;
	XeenItem &item = operator[](itemIndex);
	Character tempCharacter;

	if (item._material == 0 && item._bonusFlags == 0) {
		tempCharacter.makeItem(amount, 0, 2);
		XeenItem &tempItem = tempCharacter._armor[0];

		item._material = tempItem._material;
		item._bonusFlags = tempItem._bonusFlags;
		sound.playFX(19);
	} else {
		InventoryItems::enchantItem(itemIndex, amount);
	}
}

Common::String ArmorItems::getAttributes(XeenItem &item, const Common::String &classes) {
	Common::String elemResist, attrBonus, acBonus;
	elemResist = attrBonus = acBonus = Res.FIELD_NONE;

	if (item._material < 36) {
		int resistence = Res.ELEMENTAL_RESISTENCES[item._material];
		if (resistence > 0) {
			int eCategory = ELEM_FIRE;
			while (eCategory < ELEM_MAGIC && Res.ELEMENTAL_CATEGORIES[eCategory] < item._material)
				++eCategory;

			elemResist = Common::String::format(Res.ATTR_XY_BONUS, resistence,
				Res.ELEMENTAL_NAMES[eCategory]);
		}
	} else if (item._material >= 59) {
		int bonus = Res.ATTRIBUTE_BONUSES[item._material - 59];
		AttributeCategory aCategory = item.getAttributeCategory();
		attrBonus = Common::String::format(Res.ATTR_XY_BONUS, bonus,
			Res.ATTRIBUTE_NAMES[aCategory]);
	}

	int strength = Res.ARMOR_STRENGTHS[item._id];
	if (item._material >= 37 && item._material <= 58) {
		strength += Res.METAL_LAC[item._material - 37];
	}
	acBonus = Common::String::format("%+d", strength);

	return Common::String::format(Res.ITEM_DETAILS, classes.c_str(),
		Res.FIELD_NONE, Res.FIELD_NONE, Res.FIELD_NONE,
		elemResist.c_str(), acBonus.c_str(), attrBonus.c_str(), Res.FIELD_NONE);
}

void AccessoryItems::equipItem(int itemIndex) {
	XeenItem &item = operator[](itemIndex);

	if (item._id == 1) {
		int count = 0;
		for (uint idx = 0; idx < size(); ++idx) {
			XeenItem &i = operator[](idx);
			if (i._frame == 8)
				++count;
		}

		if (count <= 1)
			item._frame = 8;
		else
			equipError(-1, CATEGORY_ACCESSORY, itemIndex, CATEGORY_ACCESSORY);
	} else if (item._id == 2) {
		for (uint idx = 0; idx < size(); ++idx) {
			XeenItem &i = operator[](idx);
			if (i._frame == 12) {
				equipError(itemIndex, CATEGORY_ACCESSORY, idx, CATEGORY_ACCESSORY);
				return;
			}
		}
	} else if (item._id <= 7) {
		int count = 0;
		for (uint idx = 0; idx < size(); ++idx) {
			XeenItem &i = operator[](idx);
			if (i._frame == 7)
				++count;
		}

		if (count <= 1)
			item._frame = 7;
		else
			equipError(-2, CATEGORY_ACCESSORY, itemIndex, CATEGORY_ACCESSORY);
	} else {
		for (uint idx = 0; idx < size(); ++idx) {
			XeenItem &i = operator[](idx);
			if (i._frame == 11) {
				equipError(itemIndex, CATEGORY_ACCESSORY, idx, CATEGORY_ACCESSORY);
				return;
			}
		}

		item._frame = 11;
	}
}

Common::String AccessoryItems::getFullDescription(int itemIndex, int displayNum) {
	XeenItem &i = operator[](itemIndex);
	Resources &res = *getVm()->_resources;

	return Common::String::format("\f%02u%s%s%s\f%02u%s%s", displayNum,
		!i._bonusFlags ? "" : res._maeNames[i._material].c_str(),
		(i._bonusFlags & ITEMFLAG_BROKEN) ? Res.ITEM_BROKEN : "",
		(i._bonusFlags & ITEMFLAG_CURSED) ? Res.ITEM_CURSED : "",
		displayNum,
		Res.ARMOR_NAMES[i._id],
		(i._bonusFlags & (ITEMFLAG_BROKEN | ITEMFLAG_CURSED)) ||
			!i._bonusFlags ? "\b " : ""
	);
}

/*
* Returns a text string listing all the stats/attributes of a given item
*/
Common::String AccessoryItems::getAttributes(XeenItem &item, const Common::String &classes) {
	Common::String elemResist, attrBonus;
	elemResist = attrBonus = Res.FIELD_NONE;

	if (item._material < 36) {
		int resistence = Res.ELEMENTAL_RESISTENCES[item._material];
		if (resistence > 0) {
			int eCategory = ELEM_FIRE;
			while (eCategory < ELEM_MAGIC && Res.ELEMENTAL_CATEGORIES[eCategory] < item._material)
				++eCategory;

			elemResist = Common::String::format(Res.ATTR_XY_BONUS, resistence,
				Res.ELEMENTAL_NAMES[eCategory]);
		}
	} else if (item._material >= 59) {
		int bonus = Res.ATTRIBUTE_BONUSES[item._material - 59];
		AttributeCategory aCategory = item.getAttributeCategory();
		attrBonus = Common::String::format(Res.ATTR_XY_BONUS, bonus,
			Res.ATTRIBUTE_NAMES[aCategory]);
	}

	return Common::String::format(Res.ITEM_DETAILS, classes.c_str(),
		Res.FIELD_NONE, Res.FIELD_NONE, Res.FIELD_NONE,
		elemResist.c_str(), Res.FIELD_NONE, attrBonus.c_str(), Res.FIELD_NONE);
}

/*------------------------------------------------------------------------*/

Common::String MiscItems::getFullDescription(int itemIndex, int displayNum) {
	XeenItem &i = operator[](itemIndex);
	Resources &res = *getVm()->_resources;

	return Common::String::format("\f%02u%s%s%s\f%02u%s%s", displayNum,
		!i._bonusFlags ? "" : res._maeNames[i._material].c_str(),
		(i._bonusFlags & ITEMFLAG_BROKEN) ? Res.ITEM_BROKEN : "",
		(i._bonusFlags & ITEMFLAG_CURSED) ? Res.ITEM_CURSED : "",
		displayNum,
		Res.ARMOR_NAMES[i._id],
		(i._bonusFlags & (ITEMFLAG_BROKEN | ITEMFLAG_CURSED)) ||
			!i._id ? "\b " : ""
	);
}

Common::String MiscItems::getAttributes(XeenItem &item, const Common::String &classes) {
	Common::String specialPower = Res.FIELD_NONE;
	Spells &spells = *getVm()->_spells;

	if (item._id) {
		specialPower = spells._spellNames[Res.MISC_SPELL_INDEX[item._id]];
	}

	return Common::String::format(Res.ITEM_DETAILS, classes.c_str(),
		Res.FIELD_NONE, Res.FIELD_NONE, Res.FIELD_NONE, Res.FIELD_NONE, Res.FIELD_NONE,
		Res.FIELD_NONE, specialPower.c_str());
}
/*------------------------------------------------------------------------*/

InventoryItemsGroup::InventoryItemsGroup(InventoryItems &weapons, InventoryItems &armor,
		InventoryItems &accessories, InventoryItems &misc) {
	_itemSets[0] = &weapons;
	_itemSets[1] = &armor;
	_itemSets[2] = &accessories;
	_itemSets[3] = &misc;
}

InventoryItems &InventoryItemsGroup::operator[](ItemCategory category) {
	return *_itemSets[category];
}

void InventoryItemsGroup::breakAllItems() {
	for (int idx = 0; idx < INV_ITEMS_TOTAL; ++idx) {
		if ((*_itemSets[0])[idx]._id != 34) {
			(*_itemSets[0])[idx]._bonusFlags |= ITEMFLAG_BROKEN;
			(*_itemSets[0])[idx]._frame = 0;
		}

		(*_itemSets[1])[idx]._bonusFlags |= ITEMFLAG_BROKEN;
		(*_itemSets[2])[idx]._bonusFlags |= ITEMFLAG_BROKEN;
		(*_itemSets[3])[idx]._bonusFlags |= ITEMFLAG_BROKEN;
		(*_itemSets[1])[idx]._frame = 0;
		(*_itemSets[2])[idx]._frame = 0;
	}
}

} // End of namespace Xeen
