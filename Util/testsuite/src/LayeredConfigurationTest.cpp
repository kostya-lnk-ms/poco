//
// LayeredConfigurationTest.cpp
//
// $Id: //poco/1.3/Util/testsuite/src/LayeredConfigurationTest.cpp#3 $
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "LayeredConfigurationTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Util/LayeredConfiguration.h"
#include "Poco/Util/MapConfiguration.h"
#include "Poco/AutoPtr.h"
#include "Poco/Exception.h"
#include <algorithm>


using Poco::Util::AbstractConfiguration;
using Poco::Util::LayeredConfiguration;
using Poco::Util::MapConfiguration;
using Poco::AutoPtr;
using Poco::NotFoundException;
using Poco::RuntimeException;


LayeredConfigurationTest::LayeredConfigurationTest(const std::string& name): CppUnit::TestCase(name)
{
}


LayeredConfigurationTest::~LayeredConfigurationTest()
{
}


void LayeredConfigurationTest::testEmpty()
{
	AutoPtr<LayeredConfiguration> pLC = new LayeredConfiguration;
	
	AbstractConfiguration::Keys keys;
	pLC->keys(keys);
	assert (keys.empty());
	
	assert (!pLC->hasProperty("foo"));
	try
	{
		pLC->setString("foo", "bar");
		fail("empty LayeredConfiguration - must throw");
	}
	catch (RuntimeException&)
	{
	}
	
	try
	{
		std::string s = pLC->getString("foo");
		fail("empty LayeredConfiguration - must throw");
	}
	catch (NotFoundException&)
	{
	}
}


void LayeredConfigurationTest::testOneLayer()
{
	AutoPtr<LayeredConfiguration> pLC = new LayeredConfiguration;
	AutoPtr<MapConfiguration> pMC = new MapConfiguration;
	
	pMC->setString("prop1", "value1");
	pMC->setString("prop2", "value2");
	
	pLC->addWriteable(pMC, 0);

	AbstractConfiguration::Keys keys;
	pLC->keys(keys);
	assert (keys.size() == 2);
	assert (std::find(keys.begin(), keys.end(), "prop1") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop2") != keys.end());
	
	assert (pLC->getString("prop1") == "value1");
	assert (pLC->getString("prop2") == "value2");

	pLC->setString("prop3", "value3");
	assert (pLC->getString("prop3") == "value3");
}


void LayeredConfigurationTest::testTwoLayers()
{
	AutoPtr<LayeredConfiguration> pLC = new LayeredConfiguration;
	AutoPtr<MapConfiguration> pMC1 = new MapConfiguration;
	AutoPtr<MapConfiguration> pMC2 = new MapConfiguration;
	
	pMC1->setString("prop1", "value1");
	pMC1->setString("prop2", "value2");
	pMC2->setString("prop2", "value3");
	pMC2->setString("prop3", "value4");
	
	pLC->add(pMC1, 0);
	pLC->addWriteable(pMC2, 1);

	AbstractConfiguration::Keys keys;
	pLC->keys(keys);
	assert (keys.size() == 3);
	assert (std::find(keys.begin(), keys.end(), "prop1") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop2") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop3") != keys.end());
	
	assert (pLC->getString("prop1") == "value1");
	assert (pLC->getString("prop2") == "value2");
	assert (pLC->getString("prop3") == "value4");

	pLC->setString("prop4", "value4");
	assert (pLC->getString("prop4") == "value4");

	assert (!pMC1->hasProperty("prop4"));
	assert (pMC2->hasProperty("prop4"));

	pLC->setString("prop1", "value11");
	assert (pLC->getString("prop1") == "value1");
	assert (pMC2->getString("prop1") == "value11");
}


void LayeredConfigurationTest::testThreeLayers()
{
	AutoPtr<LayeredConfiguration> pLC = new LayeredConfiguration;
	AutoPtr<MapConfiguration> pMC1 = new MapConfiguration;
	AutoPtr<MapConfiguration> pMC2 = new MapConfiguration;
	AutoPtr<MapConfiguration> pMC3 = new MapConfiguration;
	
	pMC1->setString("prop1", "value1");
	pMC1->setString("prop2", "value2");
	pMC1->setString("prop3", "value3");
	pMC2->setString("prop2", "value4");
	pMC2->setString("prop4", "value5");
	pMC3->setString("prop5", "value6");
	pMC3->setString("prop1", "value7");
	
	pLC->add(pMC1, 0);
	pLC->add(pMC2, 1);
	pLC->add(pMC3, -1);
	
	assert (pLC->getString("prop1") == "value7");
	assert (pLC->getString("prop2") == "value2");
	assert (pLC->getString("prop3") == "value3");
	assert (pLC->getString("prop4") == "value5");
	assert (pLC->getString("prop5") == "value6");
}


void LayeredConfigurationTest::testRemove()
{
	AutoPtr<LayeredConfiguration> pLC = new LayeredConfiguration;
	AutoPtr<MapConfiguration> pMC1 = new MapConfiguration;
	AutoPtr<MapConfiguration> pMC2 = new MapConfiguration;
	
	pMC1->setString("prop1", "value1");
	pMC1->setString("prop2", "value2");
	pMC2->setString("prop2", "value3");
	pMC2->setString("prop3", "value4");
	
	pLC->add(pMC1, 0);
	pLC->add(pMC2, -1);

	AbstractConfiguration::Keys keys;
	pLC->keys(keys);
	assert (keys.size() == 3);
	assert (std::find(keys.begin(), keys.end(), "prop1") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop2") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop3") != keys.end());
	
	assert (pLC->getString("prop1") == "value1");
	assert (pLC->getString("prop2") == "value3");
	assert (pLC->getString("prop3") == "value4");

	pLC->remove(pMC2);
	keys.clear();
	pLC->keys(keys);
	assert (keys.size() == 2);
	
	assert (pLC->getString("prop1") == "value1");
	assert (pLC->getString("prop2") == "value2");
}


void LayeredConfigurationTest::setUp()
{
}


void LayeredConfigurationTest::tearDown()
{
}


CppUnit::Test* LayeredConfigurationTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("LayeredConfigurationTest");

	CppUnit_addTest(pSuite, LayeredConfigurationTest, testEmpty);
	CppUnit_addTest(pSuite, LayeredConfigurationTest, testOneLayer);
	CppUnit_addTest(pSuite, LayeredConfigurationTest, testTwoLayers);
	CppUnit_addTest(pSuite, LayeredConfigurationTest, testThreeLayers);
	CppUnit_addTest(pSuite, LayeredConfigurationTest, testRemove);

	return pSuite;
}
