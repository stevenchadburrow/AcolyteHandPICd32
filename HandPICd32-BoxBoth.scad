// Both together, just for visuals

//color("#808080") {
shift = 0;


/*
shift = 35;
color("#40C040") {
translate([50,58,22]) {
cube([62,34,3],center=true);    
    
}
}
*/

/*
shift = 10;
color("#40C040") {
translate([50, 69, 22]) {
cube([50,14,4],center=true);
}
}
*/


// beginning of top half
rotate([180,0,0]) {
translate([0,-100,-20]) {
    
// top
translate([-3, -3, -2]) {
    difference() {
        cube([106, 106, 2], center = false);
        
    }
}

// back
translate([-3, -3, 0]) {
    difference() {
        cube([106, 2, 20], center = false);
        translate([3+9, -1, 5]) {
            cube([82, 4, 16], center = false);
        }
    }
}

// front
translate([-3, 101, 0]) {
    difference() {
        cube([106, 2, 20], center = false);
        translate([3+9, -1, 5]) {
            cube([82, 4, 16], center = false);
        }
    }
}

// right
translate([-3, -3, 0]) {
    difference() {
        cube([2, 106, 20], center = false);
        translate([-1, 3+9, 5]) {
            cube([4, 82, 16], center = false);
        }
    }
}

// left
translate([101, -3, 0]) {
    difference() {
        cube([2, 106, 20], center = false);
        translate([-1, 3+9, 5]) {
            cube([4, 82, 16], center = false);
        }
    }
}

// corner
translate([5, 5, 0]) {
    difference() {
        cylinder(20, 4, 4);
        translate([0, 0, -1]) {
            cylinder(21.25, 1.5, 1.5);
        }
    }
}
translate([4, -1, 0]) {
    cube([2, 3, 20], center = false);
}
translate([-1, 4, 0]) {
    cube([3, 2, 20], center = false);
}

// corner
translate([95, 5, 0]) {
    difference() {
        cylinder(20, 4, 4);
        translate([0, 0, -1]) {
            cylinder(21.25, 1.5, 1.5);
        }
    }
}
translate([94, -1, 0]) {
    cube([2, 3, 20], center = false);
}
translate([99, 4, 0]) {
    cube([3, 2, 20], center = false);
}

// corner
translate([95, 95, 0]) {
    difference() {
        cylinder(20, 4, 4);
        translate([0, 0, -1]) {
            cylinder(21.25, 1.5, 1.5);
        }
    }
}
translate([94, 99, 0]) {
    cube([2, 3, 20], center = false);
}
translate([99, 94, 0]) {
    cube([3, 2, 20], center = false);
}

// corner
translate([5, 95, 0]) {
    difference() {
        cylinder(20, 4, 4);
        translate([0, 0, -1]) {
            cylinder(21.25, 1.5, 1.5);
        }
    }
}
translate([4, 99, 0]) {
    cube([2, 3, 20], center = false);
}
translate([-1, 94, 0]) {
    cube([3, 2, 20], center = false);
}
     
// bottom
translate([-3, -3, -2]) {
    difference() {
        cube([106, 106, 2], center = false);
        translate([3+5, 3+5, -1]) {
            cylinder(4, 3, 3);
        }
        translate([3+95, 3+5, -1]) {
            cylinder(4, 3, 3);
        }
        translate([3+95, 3+95, -1]) {
            cylinder(4, 3, 3);
        }
        translate([3+5, 3+95, -1]) {
            cylinder(4, 3, 3);
        }
    }
}

// front
translate([-3, -3, 0]) {
    difference() {
        cube([106, 2, 5], center = false);
        translate([73.5+3, -0.5, 0]) {
            cube([17, 3, 6], center = false);
        }
    }
}

// back
translate([-3, 101, 0]) {
    difference() {
        cube([106, 2, 5], center = false);
        translate([73.5+3, -0.5, 0]) {
            cube([17, 3, 6], center = false);
        }
    }
}

// left
translate([-3, -3, 0]) {
    difference() {
        cube([2, 106, 5], center = false);
        translate([-0.5, 73.5+3, 0]) {
            cube([3, 17, 6], center = false);
        }
    }
}

// right
translate([101, -3, 0]) {
    difference() {
        cube([2, 106, 5], center = false);
        translate([-0.5, 73.5+3, 0]) {
            cube([3, 17, 6], center = false);
        }
    }
}

// front left corner
translate([5, 5, 0]) {
    difference() {
        cylinder(3.25, 4, 4);
        translate([0, 0, -1]) {
            cylinder(4.5, 1.5, 1.5);
        }
    }
}
translate([4, -1, 0]) {
    cube([2, 3, 3.25], center = false);
}
translate([-1, 4, 0]) {
    cube([3, 2, 3.25], center = false);
}

// front right corner
translate([95, 5, 0]) {
    difference() {
        cylinder(3.25, 4, 4);
        translate([0, 0, -1]) {
            cylinder(4.5, 1.5, 1.5);
        }
    }
}
translate([94, -1, 0]) {
    cube([2, 3, 3.25], center = false);
}
translate([99, 4, 0]) {
    cube([3, 2, 3.25], center = false);
}

// back right corner
translate([95, 95, 0]) {
    difference() {
        cylinder(3.25, 4, 4);
        translate([0, 0, -1]) {
            cylinder(4.5, 1.5, 1.5);
        }
    }
}
translate([94, 99, 0]) {
    cube([2, 3, 3.25], center = false);
}
translate([99, 94, 0]) {
    cube([3, 2, 3.25], center = false);
}

// back left corner
translate([5, 95, 0]) {
    difference() {
        cylinder(3.25, 4, 4);
        translate([0, 0, -1]) {
            cylinder(4.5, 1.5, 1.5);
        }
    }
}
translate([4, 99, 0]) {
    cube([2, 3, 3.25], center = false);
}
translate([-1, 94, 0]) {
    cube([3, 2, 3.25], center = false);
} 

translate([4, 99, 0]) {
    cube([2, 3, 20], center = false);
}
translate([-1, 94, 0]) {
    cube([3, 2, 20], center = false);
}
}}
// end of top half



// beginning of bottom half
translate([0,0,-5]) {

// bottom
translate([-3, -3, -2]) {
    difference() {
        cube([106, 106, 2], center = false);
        translate([3+5, 3+5, -1]) {
            cylinder(4, 3, 3);
        }
        translate([3+95, 3+5, -1]) {
            cylinder(4, 3, 3);
        }
        translate([3+95, 3+95, -1]) {
            cylinder(4, 3, 3);
        }
        translate([3+5, 3+95, -1]) {
            cylinder(4, 3, 3);
        }
    }
}

// front
translate([-3, -3, 0]) {
    difference() {
        cube([106, 2, 5], center = false);
        translate([12, -5, 2]) {
            cube([82, 10, 4], center = false);
        }
    }
}

// back
translate([-3, 101, 0]) {
    difference() {
        cube([106, 2, 5], center = false);
        translate([12, -5, 2]) {
            cube([82, 10, 4], center = false);
        }
    }
}

// left
translate([-3, -3, 0]) {
    difference() {
        cube([2, 106, 5], center = false);
        translate([-5, 12, 2]) {
            cube([10, 82, 4], center = false);
        }
    }
}

// right
translate([101, -3, 0]) {
    difference() {
        cube([2, 106, 5], center = false);
        translate([-5, 12, 2]) {
            cube([10, 82, 4], center = false);
        }
    }
}

// front left corner
translate([5, 5, 0]) {
    difference() {
        cylinder(3.25, 4, 4);
        translate([0, 0, -1]) {
            cylinder(4.5, 1.5, 1.5);
        }
    }
}
translate([4, -1, 0]) {
    cube([2, 3, 3.25], center = false);
}
translate([-1, 4, 0]) {
    cube([3, 2, 3.25], center = false);
}

// front right corner
translate([95, 5, 0]) {
    difference() {
        cylinder(3.25, 4, 4);
        translate([0, 0, -1]) {
            cylinder(4.5, 1.5, 1.5);
        }
    }
}
translate([94, -1, 0]) {
    cube([2, 3, 3.25], center = false);
}
translate([99, 4, 0]) {
    cube([3, 2, 3.25], center = false);
}

// back right corner
translate([95, 95, 0]) {
    difference() {
        cylinder(3.25, 4, 4);
        translate([0, 0, -1]) {
            cylinder(4.5, 1.5, 1.5);
        }
    }
}
translate([94, 99, 0]) {
    cube([2, 3, 3.25], center = false);
}
translate([99, 94, 0]) {
    cube([3, 2, 3.25], center = false);
}

// back left corner
translate([5, 95, 0]) {
    difference() {
        cylinder(3.25, 4, 4);
        translate([0, 0, -1]) {
            cylinder(4.5, 1.5, 1.5);
        }
    }
}
translate([4, 99, 0]) {
    cube([2, 3, 3.25], center = false);
}
translate([-1, 94, 0]) {
    cube([3, 2, 3.25], center = false);
}
}
// end of bottom half

//}