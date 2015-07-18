#include"CellularAutomataImplement.h"
#include<omp.h>
#include<Windows.h>
#include<locale>
#include<codecvt>
#include<boost/property_tree/ptree.hpp>
#include<boost/property_tree/xml_parser.hpp>  
#include<boost/typeof/typeof.hpp>

using namespace CA;

ElementCollect& CA::getElements()
{
	static ElementCollect ec;
	return ec;
}

Machine& CA::getCA()
{
	static Machine ca;
	return ca;
}

Machine::Machine() :
	data(boost::extents[50 + 2][50 + 2]),
	buffer(boost::extents[50 + 2][50 + 2]),
	pThread(nullptr)
{
	Point pos;
	pos.x = 25;
	pos.y = 25;
	add_element(getElements().get(L"»¬Ïè»ú"), pos);
}

Machine::~Machine()
{
	if (flag_auto_update)
	{
		stop_auto_update();
	}
}

void Machine::update()
{
	update_by_step();
}

void Machine::begin_auto_update()
{
	if (!flag_auto_update)
	{
		flag_auto_update = true;
		auto auto_update = [&]() 
		{	
			while (flag_auto_update)
			{
				update_by_step();
				Sleep(400);
			}
		return;
		};
		pThread = std::make_unique<std::thread>(auto_update);
	}
	else
	{
		std::abort();
	}
}

void Machine::stop_auto_update()
{
	if (flag_auto_update)
	{
		flag_auto_update = false;
		pThread->join();
	}
	else
	{
		std::abort();
	}
}

void Machine::set_burn(int burn)
{
	config_mutex.lock();
	this->burn = burn;
	config_mutex.unlock();
}

void Machine::set_live(int live)
{
	config_mutex.lock();
	this->live = live;
	config_mutex.unlock();
}

int Machine::get_burn()
{
	return burn;
}

int Machine::get_live()
{
	return live;
}

void CA::Machine::add_element(const Element & ele, const Point& pos)
{
	ele.for_each_point([&](Point point)
	{
		data[point.x + pos.x][point.y + pos.y] = alive;
	});
}

void Machine::for_each(std::function<void(const unit, const int, const int)> func)
{
	data_mutex.lock();
	for (int i = 1;i < 51;++i)
	{
		for (int j = 1;j < 51;++j)
		{
			func(data[i][j], i, j);
		}
	}
	data_mutex.unlock();
}

inline void Machine::update_by_step()
{
	#pragma omp parallel for
	for (int i = 1;i < 51;++i)
	{
		for (int j = 1;j < 51;++j)
		{
			int count = 
				data[i - 1][j - 1] +
				data[i - 1][j] +
				data[i - 1][j + 1] +
				data[i][j - 1] +
				data[i][j + 1] +
				data[i + 1][j - 1] +
				data[i + 1][j] +
				data[i + 1][j + 1];

			if (count == burn)
			{
				buffer[i][j] = 1;
			}
			else if (count == live)
			{
				buffer[i][j] = data[i][j]; 
			}
			else
			{
				buffer[i][j] = 0;
			}
		}
	}

	data_mutex.lock();
	boost::swap(data, buffer);
	data_mutex.unlock();
}

inline void Machine::set_alive(const int i, const int j)
{
	if (i > 0 && i < 51 && j > 0 && j < 51)
	{
		data[i][j] = alive;
	}
}

CA::Element::Element(const std::vector<Point>& data)
{
	this->data = data;
}

void Element::revolve()
{
	for (auto& i : data)
	{
		i.y *= -1;
		std::swap(i.x, i.y);
	}
}

void CA::Element::for_each_point(std::function<void(Point)> func) const
{
	std::for_each(data.begin(), data.end(), func);
}

CA::ElementCollect::ElementCollect()
{
	load_from_xml("resource.xml");
}

Element CA::ElementCollect::get(std::wstring name)
{
	return data[name];
}

void CA::ElementCollect::load_from_xml(std::wstring wpath)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::string path = conv.to_bytes(wpath);
	load_from_xml(path);
}

void CA::ElementCollect::load_from_xml(std::string path)
{
	using namespace boost::property_tree;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	ptree root;
	read_xml(path, root);
	auto resource = root.get_child("resource");
	std::for_each(resource.begin(), resource.end(), [&](auto i)
	{
		std::string name = i.second.get<std::string>("name");
		std::wstring wname = conv.from_bytes(name);
		ptree points = i.second.get_child("points");
		std::vector<Point> vecpoint;
		std::for_each(points.begin(), points.end(), [&](auto j)
		{
			CA::Point point;
			point.x = j.second.get<int>("x");
			point.y = j.second.get<int>("y");
			vecpoint.push_back(point);
		});
		data[wname] = Element(vecpoint);
	});
}
