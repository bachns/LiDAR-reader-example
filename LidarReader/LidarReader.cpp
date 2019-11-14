/*------------------------------------------------------------------------------
   Copyright (C) 2019 by Bach Nguyen Sy
   Unauthorized copying of this file via any medium is strictly prohibited
   bachns.dev@gmail.com | github.com/bachns.dev
--------------------------------------------------------------------------------
 File name:    LidarReader.cpp
 Date created: Thursday, November 14, 2019
 Written by Bach Nguyen Sy
*/

#include "pch.h"
#include <iostream>
#include <cstdint>
#include <pdal/Stage.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/PipelineManager.hpp>

using namespace std;
using namespace pdal;

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "Usage:\n" << argv[0] << " <filename.las> <quantity>\n";
		return -1;
	}
	
	string lasfile = argv[1];
	size_t quantity = stoull(argv[2]);

	unique_ptr<PipelineManager> manager(new PipelineManager);
	//tìm tên driver để đọc file argv[1]
	StageFactory factory;
	string driver = factory.inferReaderDriver(argv[1]);
	
	//thêm driver vào manager
	manager->addReader(driver);

	Stage* reader = manager->getStage();
	if (reader == nullptr)
	{
		std::cerr << "Could not read file " << argv[1];
		return -1;
	}

	Options options;
	Option fileNameOpt("filename", argv[1]);
	options.add(fileNameOpt);
	reader->addOptions(options);

	manager->execute();
	PointViewSet viewSet = manager->views();
	QuickInfo quickInfo = reader->preview();
	size_t totalPoints = quickInfo.m_pointCount;
	cout << "Total points: " << totalPoints << endl;
	
	size_t readCount = 0;
	for (const auto& view : viewSet)
	{
		bool hasColor = view->hasDim(Dimension::Id::Red);
		for (size_t i = 0; i < view->size(); ++i)
		{
			double x = view->getFieldAs<double>(Dimension::Id::X, i);
			double y = view->getFieldAs<double>(Dimension::Id::Y, i);
			double z = view->getFieldAs<double>(Dimension::Id::Z, i);
			uint16_t intensity =  view->getFieldAs<uint16_t>(Dimension::Id::Intensity, i);
			uint8_t returnNumber = view->getFieldAs<uint8_t>(Dimension::Id::ReturnNumber, i);
			uint8_t numberOfReturns = view->getFieldAs<uint8_t>(pdal::Dimension::Id::NumberOfReturns, i);
			uint16_t pointSourceId = view->getFieldAs<uint16_t>(pdal::Dimension::Id::PointSourceId, i);
			uint8_t classification = view->getFieldAs<uint8_t>(pdal::Dimension::Id::Classification, i);

			cout << "(" << x << ", " << y << ", " << z << ")" << endl;
			cout << " intensity: " << intensity << endl;
			cout << " return number: " << (int)returnNumber << endl;
			cout << " number of returns: " << (int)numberOfReturns << endl;
			cout << " point source id: " << pointSourceId << endl;
			cout << " classification: " << (int)classification << endl;

			if (hasColor)
			{
				uint16_t red = view->getFieldAs<uint16_t>(Dimension::Id::Red, i);
				uint16_t green = view->getFieldAs<uint16_t>(Dimension::Id::Green, i);
				uint16_t blue = view->getFieldAs<uint16_t>(Dimension::Id::Blue, i);
				cout << " color: (" << red << ", "  << green << ", " << blue << ")" << endl;
			}

			cout << endl;
			if (++readCount == quantity)
			{
				break;
			}
		}
	}

	return 0;
}