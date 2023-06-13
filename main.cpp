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
	double mu;
	double epsilon;
	double delta;
	string name;
	double scale;
	vector<int>filter;

	double relative_acceleration;
	double rotation;
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
	Force g(gravity_v * b.mass);
	g.static_force = true;
	b.add_force_local_pos(v2(), g);
    return b;
}

double read_with_default(json& where, string what, double def){
	if (where.find(what) != where.end())
    	return where[what];
    return def;
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

	config.scale = read_with_default(j, "scale", 1.0);
	config.mu = read_with_default(j, "mu", 0.0);
	config.epsilon = read_with_default(j, "epsilon", 1.0);
	config.delta = read_with_default(j, "delta", 0.1);

	config.relative_acceleration = read_with_default(j, "relative_acceleration", 1.0);
	config.rotation = read_with_default(j, "rotation", 1.0);


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


void try_configuration(Config &config, vector<Dynamic_box>& boxes){
		int n = Force::lengths.size();
		cout << "n=" << n << endl;
		Matrix A;
		Vector b(n, 0.0);
		A.set_up_as_Identity(n);

		for (int i = 0; i < boxes.size(); i++) {
			for (int k = 0; k < 4; k++) {
				if (boxes[i].corners[k].id != -1) {
					//cout << "corner " << boxes[i].corners[k].id << endl;
					Dynamic_box* collider = boxes[i].corners[k].colliding;
					v2 normal = boxes[i].corners[k].normal;
					Vector this_box_row = boxes[i].get_matrix_row(normal, boxes[i].corners[k].global, config.rotation);
					Vector other_box_row = collider->get_matrix_row(normal, boxes[i].corners[k].global, config.rotation);

					A.set_row(boxes[i].corners[k].id, this_box_row - other_box_row * config.relative_acceleration);
					//A.set_friction_row(boxes[i].corners[k].id + 1, config.mu);
					b[boxes[i].corners[k].id] = normal.dot(gravity_v);
					if (collider->dynamic)
						b[boxes[i].corners[k].id] -= normal.dot(gravity_v) * config.relative_acceleration;
				}
			}
		}
		cout << "A:               b:" << endl;
		for(int i = 0; i < n; i++){
			for(int j = 0; j < n; j++)
				cout << A.values[j][i] << " ";
			cout << "                    " << b[i] << endl;
		}
		cout << endl;

		Vector f = Vector(n, 0.0);
		solve_lemke(A, b, f);
		Vector w = A * f + b;
		double score = f * w;
		for (int i = 0; i < f.size(); i++)
			cout << f[i] << "\t";
		cout << endl;
		for (int i = 0; i < f.size(); i++)
			cout << w[i] << "\t";
		cout << endl << endl;
		cout << "score " << score << endl << endl;
		Force::lengths = f;
}


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
					cout << "collision " << i << " and " << j << " normal " << normal.x << " " << normal.y << endl;
					//v2 fric = normal.rotated(pi/2.0) * config.mu;

					Force f1(normal), f2(normal);
					//Force ffric1(fric), ffric2(fric);
					f2.make_opposite_to(f1);
					f1.parent = &boxes[i].corners[k];

					//ffric2.make_opposite_to(ffric1);


					boxes[i].add_force(c, f1);
					if (boxes[j].dynamic){
						boxes[j].add_force(c, f2);
						//boxes[j].add_force(c, ffric2);
					}

					boxes[i].corners[k].id = Force::lengths.size() - 1;
					boxes[i].corners[k].normal = normal;
					boxes[i].corners[k].colliding = &boxes[j];
					//boxes[i].add_force(c, ffric1);
					//forces.push_back(boxes[i].get_last_force());
				}
			}
		}


		//get matrix
		try_configuration(config, boxes);

		cout << endl;
	}
	Adv_image im(config.w, config.h);
	im.fill({ 255, 255, 255 });



	for (int i = 0; i < boxes.size(); i++)
		im.draw_rect(boxes[i], colors[i % 5]);

	v2 total_force;
	v2 rotation_centre;
	double rotation;
	cout << endl;
	for (int i = 0; i < boxes.size(); i++)
	{
		if (find(config.filter.begin(), config.filter.end(), i) != config.filter.end())
			continue;
		if (!boxes[i].dynamic)
			continue;

		cout << i << "th box rotation " << boxes[i].get_total_rotation() << endl;

		switch (config.style) {
		case 0:
			for (int j = 0; j < boxes[i].forces.size(); j++) {
				v2 g = boxes[i].to_global(boxes[i].forces[j].first);
				im.draw_arrow(
					g,
					g + boxes[i].forces[j].second.get() * config.scale,
					2.0
				);
				//cout << boxes[i].forces[j].second.direction.x << " " << boxes[i].forces[j].second.direction.y << endl;
			}
			break;
		case 1:
			total_force = v2();// gravity_v * boxes[i].mass;
			rotation = boxes[i].get_total_rotation();
			for (int j = 0; j < boxes[i].forces.size(); j++) {
				total_force += boxes[i].forces[j].second.get();
			}
			im.draw_arrow(boxes[i].pos, boxes[i].pos + total_force * config.scale, 3.0);

			rotation_centre = v2(-total_force.y, total_force.x) / boxes[i].mass / rotation;

			im.draw_rect( {boxes[i].pos + rotation_centre, v2(6.0, 6.0)}, {128, 128, 128} );
			im.draw_arrow(
				boxes[i].pos + rotation_centre + v2(24.0, 0), boxes[i].pos + rotation_centre + v2(24.0, 0) + v2(0, 1) * double(int(rotation >= 1e-7) - int(rotation <= -1e-7)) * 24.0,
				3.0,
				{128, 128, 128}
				
				);

			cout << i << "th box total force " << total_force.x << " " << total_force.y << endl; 
			break;
		case 2:
			/*im.draw_arrow(
					boxes[i].pos,
					boxes[i].pos + gravity_v * config.scale,
					2.0
			);*/
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
			total_force = v2(); //gravity_v * boxes[i].mass;
			for (int j = 0; j < boxes[i].forces.size(); j++) {
				total_force += boxes[i].forces[j].second.get();
			}
			im.draw_arrow(boxes[i].pos, boxes[i].pos + total_force / boxes[i].mass * config.scale, 3.0);
			break;
		}


	}
	cout << endl << "Forces:" << endl;
	for (int i = 0; i < Force::lengths.size(); i++) {
		cout << Force::lengths[i] << endl;
	}

	file_name = "out/" + config.name + ".png";
	im.write(file_name.c_str());

	//system("pause");
	return 0;
}
