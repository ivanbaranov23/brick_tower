//#include "stdafx.h"
#include "src/physics/dynamic_box.h"
#include "src/images/adv_image.h"
#include "src/utilities.h"

#include "src/math/lemke_solver.h"

#include <iostream>

#include "src/json.hpp"
#include <fstream>
#include <exception>


using json = nlohmann::json;


struct Config {
	int w, h;
	int style;
	string name;
	double scale;
	vector<int>filter;
};


Dynamic_box read_box(json box, int mode) {
    /*if (mode == 1){
        v2 pos = v2(box[0], box[1]);
	    v2 size = v2(box[2], box[3]);
	    double mass = box[4];

	    double rot = 0.0;
	    if (box.size() == 6)
		    rot = box[5];
        return Dynamic_box({ pos, size, rot }, mass);
    }*/
    
    v2 pos = v2(box["pos"][0], box["pos"][1]);
    v2 size = v2(box["size"][0], box["size"][1]);
    double mass = box["mass"];
        
    double rotation = 0.0;
    if (box.find("rotation") != box.end())
        rotation = box["rotation"];
        
    Dynamic_box b({pos, size, rotation}, mass);
    
    if (box.find("forces") != box.end()){
        for (auto it = box["forces"].begin(); it != box["forces"].end(); ++it)
        {
                v2 l_pos = v2((*it)[0][0], (*it)[0][1]);
                v2 dir = v2((*it)[1][0], (*it)[1][1]);
                //v2 g_pos = b.to_global(l_pos);

                Force f(dir);
                f.add_new_length(dir.length());

                b.forces.push_back({l_pos, f});
        }
            
    }
    return b;
}
void read_input(string file_name, std::vector<Dynamic_box>& boxes, Config& config, int mode) {
	ifstream file(file_name);
	if (!file.is_open()){
		cout << "Can't open file " << file_name << endl;
		throw std::invalid_argument("bad file path");
	}
	json j = json::parse(file);
	
	config.w = j["width"];
	config.h = j["height"];
	config.style = j["style"];
	config.name = j["name"];
    if (j.find("scale") != j.end())
    	config.scale = j["scale"];
    else
        config.scale = 1.0;

	if ((j.find("filter") != j.end()) && (j["filter"].size() != 0)) {
		for (json::iterator it = j["filter"].begin(); it != j["filter"].end(); ++it) {
			config.filter.push_back(*it);
		}
	}

    if (j.find("floor") != j.end()){
	    Dynamic_box floor = read_box(j["floor"], mode);
	    floor.dynamic = false;
	    boxes.push_back(floor);
    }

	for (json::iterator it = j["boxes"].begin(); it != j["boxes"].end(); ++it) {
		boxes.push_back(read_box(*it, mode));
	}
};


int main(int argc, char* argv[])
{
	//                plan
	//read from json
	//set up
	//get matrix               -done
	//get forces               -done
	//draw                     -kinda done

	int mode;
	if (argc > 1)
		mode = atoi(argv[1]);
	else{
		cout << "input mode (0 or 1, 0 for straight task): ";
		cin >> mode;
	}

    string file_name;
	if (argc > 2)
		file_name = string(argv[2]);
	else{
	    cout << "input file name (abc will become in/abc.json): ";
		cin >> file_name;
	}
	file_name = "in/" + file_name + ".json";

	Config config;
	std::vector<Dynamic_box> boxes;

    read_input(file_name, boxes, config, mode);

	Pixel colors[5] = {
		{ 255, 0, 0 },
	{ 255, 255, 0 },
	{ 0, 255, 255 },
	{ 255, 0, 255 },
	{ 0, 255, 0 }
	};

	if (mode == 0) {

		Adv_image im2(config.w, config.h);
		//Adv_image im3(config.w, config.h);
		for (int i = 0; i < boxes.size(); i++) {
			im2.draw_rect(boxes[i], colors[i % 5]);
			//im3.draw_rect(boxes[i], colors[i % 5]);
		}


		for (int i = 0; i < boxes.size(); i++) {
			v2 total_accel = boxes[i].get_total_force();
			double total_rotation = boxes[i].get_total_rotation();
			
			im2.draw_arrow(boxes[i].pos, boxes[i].pos + total_accel * config.scale, 2.0);
			im2.draw_circle(boxes[i].pos, sgn(total_rotation));
			cout << "box #" << i << "\ntotal force (" << total_accel.x << ", " << total_accel.y << ")\nrotation acceleration: " << total_rotation << endl;
		}
        im2.write("out/all_forces.png");
	}
	else if (mode == 1) {

		//collision detection
		//setting forces
		for (int i = 0; i < boxes.size(); i++) {
			for (int j = 0; j < boxes.size(); j++) {
				if ((i == j) || (!boxes[i].dynamic) || (!boxes[i].intersect(boxes[j]))) continue;

				for (int k = 0; k < 4; k++) {
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
					cout << boxes[i].corners[k].id << " " << boxes[i].corners[k].normal.x << " " << boxes[i].corners[k].normal.y << endl;
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

	}
	Adv_image im(config.w, config.h);
	im.fill({ 255, 255, 255 });



	for (int i = 0; i < boxes.size(); i++)
		im.draw_rect(boxes[i], colors[i % 5]);

	v2 total_force;
	for (int i = 0; i < boxes.size(); i++)
	{
		if (find(config.filter.begin(), config.filter.end(), i) != config.filter.end())
			continue;
		if (!boxes[i].dynamic)
			continue;

		switch (config.style) {
		case 0:
			im.draw_arrow(
					boxes[i].pos,
					boxes[i].pos + gravity_v * boxes[i].mass * config.scale,
					2.0
			);
			for (int j = 0; j < boxes[i].forces.size(); j++) {
				v2 g = boxes[i].to_global(boxes[i].forces[j].first);
				im.draw_arrow(
					g,
					g + boxes[i].forces[j].second.get() * config.scale,
					2.0
				);
			}
			break;
		case 1:
			total_force = gravity_v * boxes[i].mass;
			for (int j = 0; j < boxes[i].forces.size(); j++) {
				total_force += boxes[i].forces[j].second.get();
			}
			im.draw_arrow(boxes[i].pos, boxes[i].pos + total_force * config.scale, 3.0);
			break;
		case 2:
			im.draw_arrow(
					boxes[i].pos,
					boxes[i].pos + gravity_v * config.scale,
					2.0
			);
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

	//system("pause");
	return 0;
}
