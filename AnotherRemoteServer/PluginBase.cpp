#include "PluginBase.hpp"

PluginBase::PluginBase()
{}

PluginBase::~PluginBase()
{}

PluginBase::PluginDATA::PluginDATA() : pluginName("")
{
	for (int i = 0; i < 8; ++i)
	{
		actions[i] = "";
	}
}

PluginBase::PluginDATA::~PluginDATA()
{
   pluginName.clear();

	for (int i = 0; i < 8; ++i)
	{
        actions[i].clear();
	}
}

bool PluginBase::getName(PluginBase::PluginDATA &data)
{
    bool ret = false;

    data = _myData;

    if (data == _myData)
    {
        ret = true;
    }

	return ret;
}

void PluginBase::setInstrumentsCollection(InstrumentsCollectionInterface *obj)
{
    _instrumentsCollection = obj;
}

PluginBase::PluginDATA::PluginDATA(const PluginDATA &obj)
{
    if (!obj.pluginName.isEmpty())
	{
		pluginName = obj.pluginName;
	}

	for (int i = 0; i < 8; ++i)
	{
        if (!obj.actions[i].isEmpty())
		{
			actions[i] = obj.actions[i];
		}
	}
}

PluginBase::PluginDATA &PluginBase::PluginDATA::operator= (const PluginBase::PluginDATA &obj)
{
	if (this == &obj)
	{
		return *this;
	}

    if (!obj.pluginName.isEmpty())
	{
		pluginName = obj.pluginName;
	}

	for (int i = 0; i < 8; ++i)
	{
        if (!obj.actions[i].isEmpty())
		{
			actions[i] = obj.actions[i];
		}
	}

	return *this;
}

int PluginBase::PluginDATA::operator== (const PluginDATA rhs)
{
    int eq = 0;

    if (pluginName == rhs.pluginName)
    {
        eq = 1;
        for (int i = 0; i < 8; ++i)
        {
            if (actions[i] != rhs.actions[i])
            {
                eq = 0;
                break;
            }
        }
    }

    return eq;
}

void PluginBase::PluginDATA::setName(const QString& data)
{
	pluginName = data;
}

void PluginBase::PluginDATA::setActionName(const unsigned int& num, const QString& data)
{
	if (num < 8)
	{
        actions[num] = data;
	}
}

void PluginBase::eventReceived(const eventType&)
{
    // Not implomented in this class
}
