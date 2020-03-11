#include "Mesh.h"

#include <vector>
#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

class IntRange final
{
public:
	IntRange(const int min, const int max) : m_range{min, max} {}

	int operator[](const int i) const { return m_range[i]; }
	array<int,2> m_range;

	class Iter final
	{
		friend class IntRange;
		Iter(const int i) : m_i(i) {}
	public:
		Iter& operator++() { m_i++; return *this; }
		int operator*() const { return m_i; }
	private:
		int m_i;
	};
	Iter begin() const { return Iter(m_range[0]); }
	Iter end() const { return Iter(m_range[1]); }
};
bool operator==(const IntRange::Iter& i0, const IntRange::Iter& i1)
{
	return *i0 == *i1;
}
bool operator!=(const IntRange::Iter& i0, const IntRange::Iter& i1)
{
	return !(i0 == i1);
}

inline int p1m3(const int i)
{
	return (i == 0) ? 1 : ((i == 1) ? 2 : 0);
}
void Mesh::calculateTopology()
{
	const int numVertices = this->numVertices();
	const int numFaces    = this->numFaces();
	const auto& faces = m_faces;

	cout << " calculate topology: F= " << numFaces << " V=" << numVertices << endl;
	/////////////////////////////////////////////////
	//Step1
	vector<int> vertexNumFaces(numVertices, 0);
	const IntRange vertexRange(0, numVertices);
	const IntRange faceRange(0, numFaces);
	for (const auto fi : faceRange)
	{
		const auto f = faces[fi].m_vis;
		for (int fvi = 0; fvi < 3; ++fvi)
			vertexNumFaces[f[fvi]]++;
	}
//	int maxNumEdges = numFaces * 3;

	/////////////////////////////////////////////////
	//Step2
	typedef pair<int, int> IntPair;
	int lastIndex = 0;
	vector<IntPair> vertexEdgesRange; vertexEdgesRange.reserve(numVertices);
	{
		for (int vi = 0; vi < numVertices; ++vi)
		{
			vertexEdgesRange.emplace_back(lastIndex, lastIndex);
			lastIndex += vertexNumFaces[vi]*2;
		}
	}
	const int maxTotalEdges = lastIndex;
	class EdgeTableEntry final 
	{
	public:
		EdgeTableEntry() : m_vi{-1}, m_fiC{-1,-1}
		{}
		int m_vi;
		array<int,2> m_fiC;
	};
	vector<EdgeTableEntry> edgeTable(maxTotalEdges);

	/////////////////////////////////////////////////
	//Step3
	for (int fi=0; fi<numFaces; ++fi)
	{
		const auto f = faces[fi].m_vis;
		for (int fei = 0; fei < 3; ++fei)
		{
			const auto vi0 = f[     fei ];
			const auto vi1 = f[p1m3(fei)];
			const auto viS0 = min(vi0, vi1);
			const auto viS1 = max(vi0, vi1);
			const auto fii = (vi0 < vi1) ? 0 : 1;
			auto& etis = vertexEdgesRange[viS0];
			const auto eti = etis.second;
			assert(eti - etis.first < vertexNumFaces[viS0]*2);
			assert(viS0 >= numVertices - 1 || eti < vertexEdgesRange[viS0 + 1].first);
			++etis.second;
			auto& ete = edgeTable[eti];
			edgeTable[eti].m_vi = viS1;
			edgeTable[eti].m_fiC[fii] = (fi << 2) + fei;
		}
	}
	
	int numEdges = numFaces*3;
	/////////////////////////////////////////////////
	//Step4
	for (int vi = 0; vi < numVertices; ++vi)
	{
		auto& etis = vertexEdgesRange[vi];
		const int start = etis.first;
		int end = etis.second;
		for (int i = start; i < end - 1; ++i)
		{
			const auto eteI = edgeTable[i];
			for (int j = i + 1; j < end; ++j)
			{
				const auto eteJ = edgeTable[j];
				if (eteI.m_vi == eteJ.m_vi)
				{
					for (int k = 0; k < 2; ++k)
					{
						const auto fiE = eteJ.m_fiC[k];
						if (fiE != -1)
							edgeTable[i].m_fiC[k] = fiE;
					}
					--end;
					if (j<end)
						edgeTable[j] = edgeTable[end];
					--numEdges;
					break; //We don't check for repeated / triple+ edges
				}
			}
		}
		etis.second = end;
	}

	cout << " edges: " << numEdges << " / " << numFaces * 3 << " Euler X (V+F-E)=" << numVertices - numEdges + numFaces << endl;
	
	/////////////////////////////////////////////////
	//Step5
	class VertexEdges final
	{
	public:
		void append(const int ei) { m_edgeIndices.push_back(ei); }
		int num() const { return m_edgeIndices.size(); }
		vector<int> m_edgeIndices;
	};
	vector<VertexEdges> vertexEdgess(numVertices);
	class FaceEdges final
	{
	public:
		FaceEdges() : m_feis{ -1,-1,-1 } {}
		array<int,3> m_feis;
	};
	vector<FaceEdges> faceEdges(numFaces);
	vector<Edge> edges; edges.reserve(numEdges);
	int curEdge = 0;
	for (int vi = 0; vi < numVertices; ++vi)
	{
		auto& ves = vertexEdgess[vi];
		const auto etis = vertexEdgesRange[vi];
		for (int i = etis.first; i < etis.second; ++i)
		{
			const auto ete = edgeTable[i];
			const auto ei = curEdge; curEdge++;
			const auto ovi = ete.m_vi;
			const array<int,2> fis = { ete.m_fiC[0]==-1 ? -1 : ete.m_fiC[0] >> 2, ete.m_fiC[1]==-1 ? -1 : ete.m_fiC[1] >> 2 };
			edges.emplace_back(vi, ovi, fis[0], fis[1]);
			ves.append(ei);
			vertexEdgess[ovi].append(ei);
			for (int k = 0; k < 2; ++k)
			{
				const auto fi = fis[k];
				if (fi != -1)
				{
					const auto fei = ete.m_fiC[k] & 3;
					faceEdges[fi].m_feis[fei] = ei;
				}
			}
		}
	}
	assert(edges.size() == numEdges);
}

