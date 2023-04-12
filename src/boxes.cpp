
#include "stdafx.h"
#include "Dynamic_box.h"
#include "Adv_image.h"
#include "utilities.h"

#include "lemke_solver.h"

#include <iostream>

#include "json.hpp"
#include <fstream>



using json = nlohmann::json;


struct Config {
	int w, h;
	int style;
	string name;
	double scale;
	vector<int>filter;
};


Dynamic_box read_box(json box) {
	v2 pos = v2(box[0], box[1]);
	v2 size = v2(box[2], box[3]);
	double mass = box[4];

	double rot = 0.0;
	if (box.size() == 6)
		rot = box[5];

	return Dynamic_box({ pos, size, rot }, mass);
}
void read_input(string file_name, std::vector<Dynamic_box>& boxes, Config& config) {
	ifstream file(file_name);
	json j = json::parse(file);
	
	config.w = j["width"];
	config.h = j["height"];
	config.style = j["style"];
	config.name = j["name"];
	config.scale = j["scale"];

	if ((j.find("filter") != j.end()) && (j["filter"].size() != 0)) {
		for (json::iterator it = j["filter"].begin(); it != j["filter"].end(); ++it) {
			config.filter.push_back(*it);
		}
	}

	Dynamic_box floor = read_box(j["floor"]);
	floor.dynamic = false;
	boxes.push_back(floor);

	for (json::iterator it = j["boxes"].begin(); it != j["boxes"].end(); ++it) {
		boxes.push_back(read_box(*it));
	}
};


int main()
{
	//                plan
	//read from json
	//set up
	//get matrix               -done
	//get forces               -done
	//draw                     -kinda done

	//read from json
	string file_name;
	cin >> file_name;
	if (file_name == "")
		file_name = "default";
	file_name = "in/" + file_name + ".json";

	//set up
	Config config;
	std::vector<Dynamic_box> boxes;
	read_input(file_name, boxes, config);

	

	//collision detection
	//setting forces
	for (int i = 0; i < boxes.size(); i++) {
		for (int j = 0; j < boxes.size(); j++) {
			if ((i == j)||(!boxes[i].dynamic)||(!boxes[i].intersect(boxes[j]))) continue;
			
			for (int k = 0; k < 4; k++){
				v2 c = boxes[i].get_corner(k);

				if (!boxes[j].intersect(c))continue;

				v2 normal = boxes[j].get_normal(c);
				
				Force f1(normal), f2(normal);
				f2.make_opposite_to(f1);
				boxes[i].add_force(c, f1);
				if (boxes[j].dynamic)
					boxes[j].add_force(c, f2);

				boxes[i].corners[k].id = Force::lengths.size() - 1;
				boxes[i].corners[k].normal += normal;
			}
		}
	}
	

	//get matrix
	int n = Force::lengths.size();
	Matrix A;
	Vector b(n, 0.0);
	A.set_up_as_Identity(n);

	for (int i = 0; i < boxes.size(); i++) {
		for (int k = 0; k < 4; k++) {
			if (boxes[i].corners[k].id != -1) {
				A.set_row(boxes[i].corners[k].id, boxes[i].get_matrix_row(k));
				b[boxes[i].corners[k].id] = boxes[i].corners[k].normal.dot(gravity_v);
			}
		}
	}

	for (int y = 0; y < A.h; y++) {
		for (int x = 0; x < A.w; x++) {
			cout << A.values[x][y] << "\t";
		}
		cout << endl;
	}
	
	Vector forces = Vector(n, 0.0);
	solve_lemke(A, b, forces);
	Force::lengths = forces;
	

	Adv_image im(config.w, config.h);
	im.fill({255, 255, 255});

	Pixel colors[5] = {
	{ 255, 0, 0 },
	{ 255, 255, 0 },
	{ 0, 255, 255 },
	{ 255, 0, 255 },
	{ 0, 255, 0 }
	};

	for (int i = 0; i < boxes.size(); i++)
		im.draw_rect(boxes[i], colors[i%5]);

	v2 total_force;
	for (int i = 0; i < boxes.size(); i++)
	{
		if (find(config.filter.begin(), config.filter.end(), i) != config.filter.end())
			continue;
		if (!boxes[i].dynamic)
			continue;
		
		switch (config.style) {
		case 0:
			for (int j = 0; j < boxes[i].forces.size(); j++) {
				v2 g = boxes[i].to_global(boxes[i].forces[j].first);
				im.draw_arrow(
					g,
					g + boxes[i].forces[j].second.get() * config.scale,
					2.0
				);
				//cout << g.x << " " << g.y << " " << boxes[i].forces[j].second.direction.x << " " << boxes[i].forces[j].second.direction.y << endl;
			}
			break;
		case 1:
			total_force = gravity_v;
			for (int j = 0; j < boxes[i].forces.size(); j++) {
				total_force += boxes[i].forces[j].second.get();
			}
			im.draw_arrow(boxes[i].pos, boxes[i].pos + total_force * config.scale, 3.0);
			break;
		case 2:
			for (int j = 0; j < boxes[i].forces.size(); j++) {
				v2 g = boxes[i].to_global(boxes[i].forces[j].first);
				im.draw_arrow(
					g,
					g + boxes[i].forces[j].second.get() / boxes[i].mass * config.scale,
					4.0
				);
				
			}
			break;
		case 3:
			total_force = gravity_v;
			for (int j = 0; j < boxes[i].forces.size(); j++) {
				total_force += boxes[i].forces[j].second.get();
			}
			im.draw_arrow(boxes[i].pos, boxes[i].pos + total_force / boxes[i].mass * config.scale, 3.0);
			break;
		}
		
	}

	cout << "Forces:" << endl;
	for (int i = 0; i < Force::lengths.size(); i++) {
		cout << Force::lengths[i] << endl;
	}

	file_name = "out/" + config.name + ".png";
	im.write(file_name.c_str());

	system("pause");
    return 0;
}

