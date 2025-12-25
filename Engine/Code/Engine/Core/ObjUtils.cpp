#include "ObjUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
void ParseObjFromFile(std::string const& filename, std::vector<Vertex_PCUTBN>& out_vertexes, std::vector<unsigned int>& out_indexes, bool reverse)
{
    std::string fileString = ""; 
    FileReadToString(fileString, filename);

    fileString.erase(std::remove(fileString.begin(), fileString.end(), '\r'), fileString.end());
    std::vector<std::string> lines = SplitStringOnDelimiter(fileString, '\n');
    lines = RemoveExtraLines(lines);
    ParseVertexArray(lines, out_vertexes, out_indexes, reverse);
}

std::vector<std::string> RemoveExtraLines(std::vector<std::string>& lines)
{
    std::vector<std::string> result;
    for (int i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        if (line.compare("") == 0 || line.at(0) == '#') {
        }
        else {
            result.push_back(line);
        }
    }
    return result;
}

 void ParseVertexArray(std::vector<std::string>const& lines, std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_index, bool reverse)
{
    std::vector<Vec3> positions;
    std::vector<Vec2> UVs;
    std::vector<Vec3> normals;
    //std::vector<Vertex_PCUTBN> results;
    positions.reserve(10000);
    UVs.reserve(10000);
    normals.reserve(10000);

    for (int i = 0; i < lines.size(); i++) {
        std::vector<std::string> values = SplitStringOnDelimiter(lines[i], ' ');

        if (values.size() < 4) {
            //something went wrong
        }

        std::string type = values[0];
        for (int j = 0; j < values.size(); j++) {
            if (values[j].compare("") == 0) {
                values.erase(values.begin()+j);
            }
        }
        
        if (type.compare("v") == 0) {
            Vec3 newPos = Vec3((float)atof(values[1].c_str()), (float)atof(values[2].c_str()), (float)atof(values[3].c_str()));
            positions.push_back(newPos);
        }

        if (type.compare("vn") == 0) {
            Vec3 newNormal = Vec3((float)atof(values[1].c_str()), (float)atof(values[2].c_str()), (float)atof(values[3].c_str()));
            normals.push_back(newNormal);
        }

		if (type.compare("vt") == 0) {
			Vec2 newUV = Vec2((float)atof(values[1].c_str()), (float)atof(values[2].c_str()));
			UVs.push_back(newUV);
		}

        if (type.compare("f") == 0) {
			
            AddVertsForFace(values, positions, UVs, normals, out_verts, out_index, reverse);
            
        }
    }
}

 void AddVertsForFace(std::vector<std::string>const& values, std::vector<Vec3>const& positions, std::vector<Vec2>const& UVs, std::vector<Vec3>const& normals,
 std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_index, bool reverse)
 {
     bool haveUV=false;
     bool haveNormal = false;
     int size = (int)out_verts.size();
     for (int j = 1; j < values.size(); j++) {
         std::vector<std::string> vectorValues = SplitStringOnDelimiter(values[j], '/');
         Vertex_PCUTBN vertex;
         int posIndex = (int)atoi(vectorValues[0].c_str()) - 1;
         vertex.m_position = positions[posIndex];
         if (vectorValues.size() != 1) {
			 if (vectorValues[1].compare("") != 0) {
				 int UVIndex = (int)atoi(vectorValues[1].c_str()) - 1;
				 vertex.m_uvTexCoords = UVs[UVIndex];
				 haveUV = true;
			 }
			 if (vectorValues[2].compare("") != 0) {
				 int normalIndex = (int)atoi(vectorValues[2].c_str()) - 1;
                 if(reverse)
				     vertex.m_normal = -normals[normalIndex];
                 else
                     vertex.m_normal = normals[normalIndex];
				 haveNormal = true;
			 }
         }
         
         out_verts.push_back(vertex);
     }

     
     Vec3 normal = Vec3();
	 Vec3 diffPos1 = out_verts[size + 1].m_position - out_verts[size].m_position;
	 Vec3 diffPos2 = out_verts[size + 2].m_position - out_verts[size].m_position;
     
     
     
	 if (reverse) {
	  normal = CrossProduct3D(diffPos2, diffPos1).GetNormalized();
	 }
	 else {
	  normal = CrossProduct3D(diffPos1, diffPos2).GetNormalized();
	 }
     
     for (int i = size; i < out_verts.size(); i++) {
         if (!haveNormal) {
         out_verts[i].m_normal = normal;
         }
         if (haveUV) {
             Vec2 diffUV1 = out_verts[size + 1].m_uvTexCoords - out_verts[size].m_uvTexCoords;
             Vec2 diffUV2 = out_verts[size + 2].m_uvTexCoords - out_verts[size].m_uvTexCoords;
             float r = 1/(diffUV1.x*diffUV2.y-diffUV2.x*diffUV1.y);

             out_verts[i].m_tangent = r*(diffUV2.y*diffPos1-diffUV1.y*diffPos1).GetNormalized();
             out_verts[i].m_bitTangent = r * (diffUV1.x * diffPos2 - diffUV2.x * diffPos2).GetNormalized();
             Mat44 TBN = Mat44(out_verts[i].m_normal, out_verts[i].m_tangent, out_verts[i].m_bitTangent);
             TBN.Orthonormalize_IFwd_JLeft_KUp();
             out_verts[i].m_normal = TBN.GetIBasis3D();
             out_verts[i].m_tangent = TBN.GetJBasis3D();
             out_verts[i].m_bitTangent = TBN.GetKBasis3D();
         }
         
     }


     if (reverse) {
		 out_index.push_back(size);
		 out_index.push_back(size + 2);
		 out_index.push_back(size + 1);
     }
     else {
		 out_index.push_back(size);
		 out_index.push_back(size + 1);
		 out_index.push_back(size + 2);
     }
     


     int numVerts = (int)values.size()-1;
     if (numVerts > 3) {
         for (int extra = 0; extra < numVerts - 3; extra++) {
             if (reverse) {
				 out_index.push_back(size);
                 out_index.push_back(size + 3 + extra);
				 out_index.push_back(size + 2 + extra);
             }
             else {
				 out_index.push_back(size);
				 out_index.push_back(size + 2 + extra);
				 out_index.push_back(size + 3 + extra);
             }
         }
     }
 }

