#pragma once

#include<string>
#include<mutex>
#include<thread>
#include<functional>
#include<memory>
#include<vector>
#include<map>
#include<boost\multi_array.hpp>

namespace CA
{
	struct Point
	{
		int x,y;
	};

	class Element
	{
	public:
		Element() {}
		explicit Element(const std::vector<Point>& data);
		void revolve();
		void for_each_point(std::function<void(Point)> func) const;

	private:
		std::vector<Point> data;
	};

	class ElementCollect
	{
	public:
		ElementCollect();
		Element get(std::wstring name);
		void load_from_xml(std::wstring wpath);
		void load_from_xml(std::string path);

	private:
		std::map<std::wstring, Element> data;

	};

	ElementCollect& getElements();

	class Machine;

	using unit = int;

	Machine& getCA();

	class Machine
	{
	public:
		Machine();
		~Machine();
		void update();
		void begin_auto_update();
		void stop_auto_update();
		void set_burn(int burn);
		void set_live(int live);
		int get_burn();
		int get_live();
		void add_element(const Element& ele, const Point& pos);

		void for_each(std::function<void(const unit, const int, const int)> func);

	private:
		const static unit dead = 0, alive = 1;
		int burn = 3, live = 2;
		boost::multi_array<unit, 2> data, buffer;
		std::mutex data_mutex,config_mutex;
		std::unique_ptr<std::thread> pThread;
		bool flag_auto_update = false;

	private:
		inline void update_by_step();
		inline void set_alive(const int i, const int j);
		Machine(const Machine &src) { return; }
		Machine operator=(const Machine &src) { return src; }
	};
}