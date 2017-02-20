xof 0302txt 0064
//
// DirectX file: explosionFlame.x
//
// Date/time of export: 06/25/2001 22:45:48
//

Header {
	1; // Major version
	0; // Minor version
	1; // Flags
}

Material gameOverMaterial {
	1.000000;1.000000;1.000000;1.000000;;
	4.000000;
	0.000000;0.000000;0.000000;;
	0.000000;0.000000;0.000000;;
	TextureFilename {
                "gameover.jpg,gameoverMask.bmp";
	}
}

// Original object name = "e1MeshMesh"
Mesh e1MeshMesh {
	4;
	0.0, 0.0, 0.0;,
	1.0, 0.0, 0.0;,
	1.0, 0.125, 0.0;,
	0.0, 0.125, 0.0;;
	
	2;
	3;2,1,3;,
	3;0,3,1;;

	MeshMaterialList {
		1;1;0;;
		{gameOverMaterial}
	}

	MeshNormals {
		1; // 1 normal
		0.000000;0.000000;-1.000000;;

		2;		// 28 faces
		3;0,0,0;,
		3;0,0,0;;
	}  // End of Normals

	MeshTextureCoords {
		4; // 4 texture coords
		0.0;0.0;,
		1.0;0.0;,
		1.0;1.0;,
		0.0;1.0;;
	}  // End of texture coords
} // End of Mesh
