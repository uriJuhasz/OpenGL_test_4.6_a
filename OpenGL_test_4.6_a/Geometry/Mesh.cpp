#include "Mesh.h"

#include "Utilities/Misc.h"
#include "Utilities/UninitializedAllocator.h"

#include <vector>
#include <array>
#include <algorithm>

#include <cassert>

#include <atomic>
#include <execution>
#include <iterator>

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
		typedef std::forward_iterator_tag iterator_category;
		Iter(const int i) : m_i(i) {}
	public:
		Iter() : m_i(-1) {}
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

namespace std
{
	template<> class iterator_traits<IntRange::Iter>
	{
	public:
		typedef IntRange::Iter Iter;
		typedef int difference_type;
		typedef std::forward_iterator_tag iterator_category;
	};
	template<>int distance(const IntRange::Iter a, const IntRange::Iter b)
	{
		return *b - *a;
	}
}

inline int p1m3(const int i)
{
	return (i == 0) ? 1 : ((i == 1) ? 2 : 0);
}

void calculateTopology1(Mesh& mesh)
{
	const auto& faces = mesh.m_faces;
	const auto numVertices = mesh.numVertices();
	const int numFaces = mesh.numFaces();

	const IntRange vertexRange(0, numVertices);
	const IntRange faceRange(0, numFaces);
	typedef pair<int, int> IntPair;

	/////////////////////////////////////////////////
	//Step1
	vector<int> vertexNumFaces(numVertices, 0);
	{//Step1
		for (const auto fi : faceRange)
		{
			const auto f = faces[fi].m_vis;
			for (int fvi = 0; fvi < 3; ++fvi)
				vertexNumFaces[f[fvi]]++;
		}
	}

	/////////////////////////////////////////////////
	//Step2
	int _maxTotalEdges;
	vector<IntPair> vertexEdgesRange; vertexEdgesRange.reserve(numVertices);
	{//Step2
		int lastIndex = 0;
		{
			for (int vi = 0; vi < numVertices; ++vi)
			{
				vertexEdgesRange.emplace_back(lastIndex, lastIndex);
				lastIndex += vertexNumFaces[vi] * 2;
			}
		}
		_maxTotalEdges = lastIndex;
	}
	const int maxTotalEdges = _maxTotalEdges;

	/////////////////////////////////////////////////
	//Step3
	class EdgeTableEntry final
	{
	public:
		EdgeTableEntry() : m_vi{ -1 }, m_fiC{ -1,-1 }
		{}
		int m_vi;
		array<int, 2> m_fiC;
	};

	vector<EdgeTableEntry, UninitializedAllocator<EdgeTableEntry>> edgeTable(maxTotalEdges);
//	vector<EdgeTableEntry> edgeTable(maxTotalEdges);
	{//Step3
		for (int fi = 0; fi < numFaces; ++fi)
		{
			const auto f = faces[fi].m_vis;
			for (int fei = 0; fei < 3; ++fei)
			{
				const auto vi0 = f[fei];
				const auto vi1 = f[p1m3(fei)];
				const auto viS0 = min(vi0, vi1);
				const auto viS1 = max(vi0, vi1);
				const auto fii = (vi0 < vi1) ? 0 : 1;
				auto& etis = vertexEdgesRange[viS0];
				const auto eti = etis.second;
				assert(eti - etis.first < vertexNumFaces[viS0] * 2);
				assert(viS0 >= numVertices - 1 || eti < vertexEdgesRange[viS0 + 1].first);
				++etis.second;
				auto& ete = edgeTable[eti];
				ete.m_vi = viS1;
				ete.m_fiC[fii] = (fi << 2) + fei;
				ete.m_fiC[1 - fii] = -1;
			}
		}
	}

	/////////////////////////////////////////////////
	//Step4
	int numEdgesX = numFaces * 3;
	vector<array<int, 2>> duplicatedEdges;
	{ //Step4
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
							{
								if (edgeTable[i].m_fiC[k] == -1)
								{
									edgeTable[i].m_fiC[k] = fiE;
								}
								else
								{
//									const auto fiI = edgeTable[i].m_fiC[k] >> 2;
									//const auto feiI = edgeTable[i].m_fiC[k] & 3;
//									const auto fiJ = edgeTable[j].m_fiC[k] >> 2;
//									const auto feiJ = edgeTable[j].m_fiC[k] & 3;
//									const auto& fI = faces[fiI].m_vis;
//									const auto& fJ = faces[fiJ].m_vis;
/* Duplicate edge warning - should we split?
                                   cerr << " Warning - triangle edges "
										<< fiI << ":" << feiI << " [" << fI[0] << " " << fI[1] << " " << fI[2] << "]"
										<< " and "
										<< fiJ << ":" << feiJ << " [" << fJ[0] << " " << fJ[1] << " " << fJ[2] << "]"
										<< " overlap on edge " << (k ? vi : eteJ.m_vi) << " - " << (k ? eteJ.m_vi : vi)
										<< endl;
*/
									duplicatedEdges.emplace_back(array<int, 2>{edgeTable[i].m_fiC[k], edgeTable[j].m_fiC[k]});
								}
							}
						}
						--end;
						if (j < end)
							edgeTable[j] = edgeTable[end];
						--numEdgesX;
						break; //We don't check for repeated / triple+ edges
					}
				}
			}
			etis.second = end;
		}
	}
	
	const auto numEdges = numEdgesX;

	/////////////////////////////////////////////////
	//Step5
	{//Step5
		auto& faceEdges = mesh.m_faceEdges;
		auto& edges = mesh.m_edges;
		auto& allVertexEdgeLists = mesh.m_allVertexEdgeLists;
		auto& vertexEdgeIndicesRanges = mesh.m_vertexEdgeIndicesRanges;

		faceEdges.resize(numFaces);
		edges.reserve(numEdges);
		allVertexEdgeLists.resize(numEdges * 2);
		vertexEdgeIndicesRanges.resize(numVertices);

		for (int vi = 0; vi < numVertices; ++vi)
		{
			const auto ver = vertexEdgesRange[vi];
			const auto numVertexEdges = ver.second - ver.first;
			vertexEdgeIndicesRanges[vi].m_num += numVertexEdges;
			for (int ii = ver.first; ii < ver.second; ++ii)
				vertexEdgeIndicesRanges[edgeTable[ii].m_vi].m_num += 1;
		}
		{
			int lastIndex = 0;
			for (int vi = 0; vi < numVertices; ++vi)
			{
				auto& veir = vertexEdgeIndicesRanges[vi];
				assert(lastIndex < allVertexEdgeLists.size());
				veir.m_first = lastIndex;
				lastIndex += veir.m_num;
				assert(lastIndex <= allVertexEdgeLists.size());
				veir.m_num = 0;
			}
		}

		int curEdge = 0;
		for (int vi = 0; vi < numVertices; ++vi)
		{
			auto& veir = vertexEdgeIndicesRanges[vi];
			const auto etis = vertexEdgesRange[vi];
			for (int vei = etis.first; vei < etis.second; ++vei)
			{
				const auto ete = edgeTable[vei];
				const auto ei = curEdge; curEdge++;
				const auto ovi = ete.m_vi;
				const array<int, 2> fis = { ete.m_fiC[0] == -1 ? -1 : ete.m_fiC[0] >> 2, ete.m_fiC[1] == -1 ? -1 : ete.m_fiC[1] >> 2 };
				edges.emplace_back(vi, ovi, fis[0], fis[1]);
				{
					const auto o = veir.m_num++;
					const auto wo = veir.m_first + o;
					assert(wo >= veir.m_first);
					if (vi < numVertices - 1)
						assert(wo < vertexEdgeIndicesRanges[vi + 1].m_first);
					else
						assert(wo < allVertexEdgeLists.size());
					allVertexEdgeLists[wo] = ei;
				}
				{
					auto& oveir = vertexEdgeIndicesRanges[ovi];
					const auto o = oveir.m_num++;
#ifdef _DEBUG
					const auto wo = oveir.m_first + o;
					assert(wo >= veir.m_first);
					if (ovi < numVertices - 1)
						assert(wo < vertexEdgeIndicesRanges[ovi + 1].m_first);
					else
						assert(wo < allVertexEdgeLists.size());
#endif
					allVertexEdgeLists[oveir.m_first + o] = ei;
				}
				for (int k = 0; k < 2; ++k)
				{
					const auto fi = fis[k];
					const auto fei = ete.m_fiC[k] & 3;
					if (fi != -1)
					{
						faceEdges[fi].m_feis[fei] = ei;
					}
				}
			}
		}
		assert(edges.size() == numEdges);
		for (const auto de : duplicatedEdges)
		{
			const auto fi0 = de[0] >> 2;
			const auto fei0 = de[0] & 3;
			const auto fi1 = de[1] >> 2;
			const auto fei1 = de[1] & 3;
			assert(faceEdges[fi1].m_feis[fei1] == -1);
			faceEdges[fi1].m_feis[fei1] = faceEdges[fi0].m_feis[fei0];
		}
	}
}

void Mesh::clearTopology()
{
	m_edges.clear();
	m_faceEdges.clear();
	m_vertexEdgeIndicesRanges.clear();
	m_allVertexEdgeLists.clear();
}

template<class T>class Atomic final
{
public:
	Atomic(const T t) : m_value(t) {}
	Atomic(const Atomic& other) : m_value(other.m_value.load()) {}

	atomic<T> m_value;
};

void calculateTopology2(Mesh& mesh)
{
	const auto& faces = mesh.m_faces;
	const auto numVertices = mesh.numVertices();
	const int numFaces = mesh.numFaces();

	const IntRange vertexRange(0, numVertices);
	const IntRange faceRange(0, numFaces);

	/////////////////////////////////////////////////
	//Step1
	vector<int> vertexNumFacesM(numVertices, 0);
	{//Step1
		for (const auto fi : faceRange)
		{
			const auto f = faces[fi].m_vis;
			for (int fvi = 0; fvi < 3; ++fvi)
				vertexNumFacesM[f[fvi]]++;
		}
		/*		Manual threads
		{
			const auto numHardwareThreads = std::thread::hardware_concurrency();
			const int numThreads = numHardwareThreads;
			vector<thread> threads;
			threads.reserve(numThreads);
			const int num = numFaces;
			const int numPerThread = num / numThreads;
			for (int i = 0; i < numThreads; ++i)
			{
				const auto rangeStart = numPerThread * i;
				const auto rangeEnd = (i == numThreads - 1) ? num : (rangeStart + numPerThread);
				const array<int, 2> range = { rangeStart,rangeEnd };
				threads.emplace_back(
					[&faces, &vertexNumFacesM](const auto range)
					{
						for (auto fi = range[0]; fi < range[1]; ++fi)
						{
							const auto f = faces[fi].m_vis;
							for (int fvi = 0; fvi < 3; ++fvi)
								vertexNumFacesM[f[fvi]].m_value++;
						}
					}, range);

			}
			for (auto& t : threads)
				t.join();
		}
		*/
		/*		for_each(execution::par,faceRange.begin(), faceRange.end(),
			[&faces,&vertexNumFacesM](const int fi)
			//		for (const auto fi : faceRange)
			{
				const auto f = faces[fi].m_vis;
				for (int fvi = 0; fvi < 3; ++fvi)
					vertexNumFacesM[f[fvi]].m_value++;
			});*/
	}
	/////////////////////////////////////////////////
	//Step2
	int _maxTotalEdges;
	class VertexEdgesRange final
	{
	public:
		VertexEdgesRange(const int first, const int last)
			: m_first(first)
			, m_last(last)
		{}

		VertexEdgesRange(const VertexEdgesRange& other)
			: m_first(other.m_first)
			, m_last(other.m_last.load())
		{}

		int m_first;
		atomic<int> m_last;
	};
	vector<VertexEdgesRange> vertexEdgesRangeM; vertexEdgesRangeM.reserve(numVertices);
	{//Step2
		const auto& vertexNumFaces = vertexNumFacesM;
		int lastIndex = 0;
		{
			for (int vi = 0; vi < numVertices; ++vi) //Not directly parallelizable
			{
				vertexEdgesRangeM.emplace_back(lastIndex, lastIndex);
				lastIndex += vertexNumFaces[vi] * 2;
			}
		}
		_maxTotalEdges = lastIndex;
	}
	const int maxTotalEdges = _maxTotalEdges;

	/////////////////////////////////////////////////
	//Step3
	class EdgeTableEntry final
	{
	public:
		EdgeTableEntry() : m_vi{ -1 }, m_fiC{ -1,-1 }
		{}
		int m_vi;
		array<int, 2> m_fiC;
	};
	vector<EdgeTableEntry, UninitializedAllocator<EdgeTableEntry>> edgeTableM(maxTotalEdges);
	{//Step3
//		for (int fi = 0; fi < numFaces; ++fi)
		std::for_each(execution::par, faceRange.begin(), faceRange.end(),
			[&faces,&vertexEdgesRangeM, &edgeTableM](const int fi){
				const auto f = faces[fi].m_vis;
				for (int fei = 0; fei < 3; ++fei)
				{
					const auto vi0 = f[fei];
					const auto vi1 = f[p1m3(fei)];
					const auto viS0 = min(vi0, vi1);
					const auto viS1 = max(vi0, vi1);
					const auto fii = (vi0 < vi1) ? 0 : 1;
					auto& etis = vertexEdgesRangeM[viS0];
					const auto last = etis.m_last++; //Atomic
					auto& ete = edgeTableM[last];
					ete.m_vi = viS1;
					ete.m_fiC[fii] = (fi << 2) + fei;
					ete.m_fiC[1 - fii] = -1;
				}
			});
	}
	/////////////////////////////////////////////////
	//Step4
	atomic<int> numEdgesM = numFaces * 3;
	
	mutex duplicateEdgesMutex;
	vector<array<int, 2>> duplicatedEdgesM; //Should be protected from multi-writes
	
	{ //Step4
		std::for_each(execution::par,vertexRange.begin(), vertexRange.end(),
			//for (int vi = 0; vi < numVertices; ++vi)
			[&vertexEdgesRangeM, &edgeTableM,&duplicatedEdgesM,&duplicateEdgesMutex ,&numEdgesM](const int vi)
			{
				auto& etis = vertexEdgesRangeM[vi]; //No need for atomics here
				const int start = etis.m_first;
				int end = etis.m_last;
				int delta = 0;
				for (int i = start; i < end - 1; ++i)
				{
					const auto eteI = edgeTableM[i];
					for (int j = i + 1; j < end; ++j)
					{
						const auto eteJ = edgeTableM[j];
						if (eteI.m_vi == eteJ.m_vi)
						{
							for (int k = 0; k < 2; ++k)
							{
								const auto fiE = eteJ.m_fiC[k];
								if (fiE != -1)
								{
									if (edgeTableM[i].m_fiC[k] == -1)
									{
										edgeTableM[i].m_fiC[k] = fiE;
									}
									else
									{
										const auto eteI = edgeTableM[i];
										const auto eteJ = edgeTableM[j];

										lock_guard lock(duplicateEdgesMutex);
										duplicatedEdgesM.emplace_back(array<int, 2>{eteI.m_fiC[k], eteJ.m_fiC[k]});
									}
								}
							}
							--end;
							if (j < end)
								edgeTableM[j] = edgeTableM[end];
							delta++;
							break; //We don't check for repeated / triple+ edges
						}
					}
				}
				numEdgesM -= delta;
				etis.m_last = end;
			});
	}

	const auto numEdges = numEdgesM.load();

	/////////////////////////////////////////////////
	//Step5
	{//Step5
		auto& faceEdgesM = mesh.m_faceEdges;
		auto& edgesM = mesh.m_edges;
		auto& allVertexEdgeListsM = mesh.m_allVertexEdgeLists;
		auto& vertexEdgeIndicesRangesM = mesh.m_vertexEdgeIndicesRanges;

		const auto& vertexEdgesRange = vertexEdgesRangeM;
		const auto& edgeTable = edgeTableM;

		/////////////////////////////////////////////////
		//Step 5.1
		class Step5Data final
		{
		public:
			Step5Data() {}
			int m_nextOwn = 0;
			int m_firstOwnEI = 0;
			atomic<int> m_nextOther = 0;
		};
		vector<Step5Data> step5DataM(numVertices);
		{
			//Separate vertex edges to own (where self vi is smaller in edge) and other
			for_each(execution::par, vertexRange.begin(), vertexRange.end(),
				[&vertexEdgesRange, &step5DataM, &edgeTable](const int vi) {
					const auto& ver = vertexEdgesRange[vi];
//					const auto first = ver.m_first;
//					const auto last = ver.m_last.load();
					const auto numVertexEdges = ver.m_last - ver.m_first;
					step5DataM[vi].m_nextOwn = numVertexEdges; //Count own
					for (int ii = ver.m_first; ii < ver.m_last; ++ii)
						step5DataM[edgeTable[ii].m_vi].m_nextOther++; //Count other
				});
		}
		
		/////////////////////////////////////////////////
		//Step 5.2
		{
			vertexEdgeIndicesRangesM.resize(numVertices);

			int lastIndex = 0;
			int lastEI = 0;
			for (int vi = 0; vi < numVertices; ++vi)
			{
				auto& data = step5DataM[vi];
				const auto ownEdges = data.m_nextOwn;
				const auto otherEdges = data.m_nextOther.load();
				
				data.m_firstOwnEI = lastEI;
				lastEI += data.m_nextOwn; //count

				const auto firstOwnIndex = lastIndex;
				data.m_nextOwn = firstOwnIndex;
				const auto firstOtherIndex = firstOwnIndex + ownEdges;
				data.m_nextOther = firstOtherIndex;

				const auto totalEdges = ownEdges + otherEdges;
				lastIndex += totalEdges;

				auto& veir = vertexEdgeIndicesRangesM[vi];
				veir.m_first = data.m_nextOwn;
				veir.m_num = totalEdges;

/*				auto& veir = vertexEdgeIndicesRangesM[vi];
				assert(lastIndex < allVertexEdgeListsM.size());
				veir.m_first = lastIndex;
				lastIndex += veir.m_num;
				assert(lastIndex <= allVertexEdgeListsM.size());
				veir.m_num = 0;*/
			}
		}

		/////////////////////////////////////////////////
		//Step 5.3
		faceEdgesM.resize(numFaces);
		edgesM.resize(numEdges);
		allVertexEdgeListsM.resize(numEdges * 2);

		constexpr bool useManualThreads = false;
		if (useManualThreads)
		{ //Manual threads
			const auto numHardwareThreads = std::thread::hardware_concurrency();
			const int numThreads = numHardwareThreads / 2;// +1;
			vector<thread> threads;
			threads.reserve(numThreads);
			const int num = numVertices;
			const int numPerThread = num / numThreads;
			for (int threadIndex = 0; threadIndex < numThreads; ++threadIndex)
			{
				const auto rangeStart = numPerThread * threadIndex;
				const auto rangeEnd = (threadIndex == numThreads - 1) ? num : (rangeStart + numPerThread);
				const array<int, 2> range = { rangeStart,rangeEnd };
				threads.emplace_back(
					[&step5DataM, &vertexEdgesRange, &edgeTable, &edgesM, &faceEdgesM, &allVertexEdgeListsM](const auto range)
					{
						for (int vi = range[0]; vi < range[1]; ++vi)
						{
							const auto& s5Data = step5DataM[vi];
							const auto& etis = vertexEdgesRange[vi];
							const auto first = etis.m_first;
							const auto last = etis.m_last.load();
							int ei = s5Data.m_firstOwnEI;
							auto myWriteOffet = s5Data.m_nextOwn;
							for (int vei = first; vei < last; ++vei)
							{
								const auto ete = edgeTable[vei];
								const auto ovi = ete.m_vi;
								const array<int, 2> fis = { ete.m_fiC[0] == -1 ? -1 : ete.m_fiC[0] >> 2, ete.m_fiC[1] == -1 ? -1 : ete.m_fiC[1] >> 2 };

								//Edge
								edgesM[ei] = Mesh::Edge(vi, ovi, fis[0], fis[1]);

								//Own vertex edge
								{
/*#ifndef NDEBUG
									const auto veir = vertexEdgeIndicesRangesM[vi];
									assert(myWriteOffet >= veir.m_first);
									if (vi < numVertices - 1)
										assert(myWriteOffet < vertexEdgeIndicesRangesM[vi + 1].m_first);
									else
										assert(myWriteOffet < allVertexEdgeListsM.size());
#endif*/
									allVertexEdgeListsM[myWriteOffet] = ei;
									myWriteOffet++;
								}

								//Other vertex edge
								{
									const int otherWriteOffset = step5DataM[ovi].m_nextOther++; //Atomic
/*#ifndef NDEBUG
									const auto oveir = vertexEdgeIndicesRangesM[ovi];
									assert(otherWriteOffset >= oveir.m_first);
									if (ovi < numVertices - 1)
										assert(otherWriteOffset < vertexEdgeIndicesRangesM[ovi + 1].m_first);
									else
										assert(otherWriteOffset < allVertexEdgeListsM.size());
#endif*/
									allVertexEdgeListsM[otherWriteOffset] = ei;
								}

								//Face edges - minus duplicates
								for (int k = 0; k < 2; ++k)
								{
									const auto fi = fis[k];
									const auto fei = ete.m_fiC[k] & 3;
									if (fi != -1)
									{
										faceEdgesM[fi].m_feis[fei] = ei;
									}
								}

								ei++;
							}
						}
					}
				,range);

			}
			for (auto& t : threads)
				t.join();
		}
		else
		{
			for_each(execution::par, vertexRange.begin(), vertexRange.end(),
				[&step5DataM, &vertexEdgesRange, &edgeTable, &edgesM, &faceEdgesM, &allVertexEdgeListsM](const int vi) {
					const auto& s5Data = step5DataM[vi];
					const auto& etis = vertexEdgesRange[vi];
					const auto first = etis.m_first;
					const auto last = etis.m_last.load();
					int ei = s5Data.m_firstOwnEI;
					auto myWriteOffet = s5Data.m_nextOwn;
					for (int vei = first; vei < last; ++vei)
					{
						const auto ete = edgeTable[vei];
						const auto ovi = ete.m_vi;
						const array<int, 2> fis = { ete.m_fiC[0] == -1 ? -1 : ete.m_fiC[0] >> 2, ete.m_fiC[1] == -1 ? -1 : ete.m_fiC[1] >> 2 };

						//Edge
						edgesM[ei] = Mesh::Edge(vi, ovi, fis[0], fis[1]);

						//Own vertex edge
						{
/*#ifndef NDEBUG
							const auto veir = vertexEdgeIndicesRangesM[vi];
							assert(myWriteOffet >= veir.m_first);
							if (vi < numVertices - 1)
								assert(myWriteOffet < vertexEdgeIndicesRangesM[vi + 1].m_first);
							else
								assert(myWriteOffet < allVertexEdgeListsM.size());
#endif*/
							allVertexEdgeListsM[myWriteOffet] = ei;
							myWriteOffet++;
						}

						//Other vertex edge
						{
							const int otherWriteOffset = step5DataM[ovi].m_nextOther++; //Atomic
/*#ifndef NDEBUG
							const auto oveir = vertexEdgeIndicesRangesM[ovi];
							assert(otherWriteOffset >= oveir.m_first);
							if (ovi < numVertices - 1)
								assert(otherWriteOffset < vertexEdgeIndicesRangesM[ovi + 1].m_first);
							else
								assert(otherWriteOffset < allVertexEdgeListsM.size());
#endif*/
							allVertexEdgeListsM[otherWriteOffset] = ei;
						}

						//Face edges - minus duplicates
						for (int k = 0; k < 2; ++k)
						{
							const auto fi = fis[k];
							const auto fei = ete.m_fiC[k] & 3;
							if (fi != -1)
							{
								faceEdgesM[fi].m_feis[fei] = ei;
							}
						}

						ei++;
					}
				});
		}
		assert(edgesM.size() == numEdges);
		const auto& duplicatedEdges = duplicatedEdgesM;
		for (const auto de : duplicatedEdges)
		{
			const auto fi0 = de[0] >> 2;
			const auto fei0 = de[0] & 3;
			const auto fi1 = de[1] >> 2;
			const auto fei1 = de[1] & 3;
			assert(faceEdgesM[fi1].m_feis[fei1] == -1);
			faceEdgesM[fi1].m_feis[fei1] = faceEdgesM[fi0].m_feis[fei0];
		}
	}
}

void Mesh::calculateTopology()
{
	clearTopology();
	calculateTopology2(*this);
	validateTopology();
}

#ifdef _DEBUG
#define alwaysAssert(b) assert(b)
#else
void alwaysAssert(const bool b)
{
	assert(b);
	if (!b)
		throw new int();
}
#endif

template<class C, class T> bool rangeContains(const C& c, const int start, const int end, const T v)
{
	for (int i = start; i < end; ++i)
		if (c[i] == v)
			return true;
	return false;
}
template<class C, class T> bool contains(const C& c, const T v)
{
	return rangeContains(c, 0, toInt(c.size()), v);
}
void Mesh::validateTopology()
{
	const auto& edges = m_edges;
	const auto F = numFaces();
	const auto V = numVertices();
	const int E = toInt(edges.size());

	for (int fi = 0; fi < F; ++fi)
	{
		const auto& fvis = m_faces[fi].m_vis;
		const auto& feis = m_faceEdges[fi].m_feis;
		for (int fei = 0; fei < 3; ++fei)
		{
			const auto ei = feis[fei];
			alwaysAssert(0 <= ei && ei < E);
			const auto& e = m_edges[ei];
			const auto vi0 = fvis[fei];
			const auto vi1 = fvis[p1m3(fei)];
			const auto pi = (vi0 <= vi1) ? 0 : 1;
			const array<int, 2> fviS{ std::min(vi0, vi1), std::max(vi0, vi1) };
			alwaysAssert(e.m_vis == fviS);
//			alwaysAssert(e.m_fis[pi] == fi); Does not hold for duplicate edges
			alwaysAssert(e.m_fis[pi] != -1);
		}
	}

	const auto& vertexEdgeIndicesRanges = m_vertexEdgeIndicesRanges;
	const auto& allVertexEdgeLists = m_allVertexEdgeLists;
	for (int vi = 0; vi < V; ++vi)
	{
		const auto veir = vertexEdgeIndicesRanges[vi];
		const auto first = veir.m_first;
		alwaysAssert(0 <= first && first < allVertexEdgeLists.size());
		const auto num = veir.m_num;
		const auto end = first + num;
		alwaysAssert(0 <= end&& end <= allVertexEdgeLists.size());
		alwaysAssert(0 <= first);
		if (0 < vi)
			alwaysAssert(vertexEdgeIndicesRanges[vi - 1].m_first + vertexEdgeIndicesRanges[vi - 1].m_num == first);

		if (vi<V-1)
			alwaysAssert(end == vertexEdgeIndicesRanges[vi + 1].m_first);
		for (int vei = first; vei < end; ++vei)
		{
			const auto ei = allVertexEdgeLists[vei];
			alwaysAssert(0 <= ei && ei < E);
			const auto& e = edges[ei];
			alwaysAssert(e.m_vis[0] == vi || e.m_vis[1] == vi);
			for (int j = vei + 1; j < end; ++j)
				alwaysAssert(allVertexEdgeLists[j] != ei);//unique edge
		}
	}

	for (int ei = 0; ei < E; ++ei)
	{
		const auto& e = edges[ei];
		assert(e.m_vis[0] < e.m_vis[1]);
		for (int evi = 0; evi < 2; ++evi)
		{
			const auto vi = e.m_vis[evi];
			alwaysAssert(0 <= vi && vi < V);
			const auto veir = vertexEdgeIndicesRanges[vi];
			const auto end = veir.m_first + veir.m_num;
			alwaysAssert(rangeContains(allVertexEdgeLists, veir.m_first, end, ei));
		}
		for (int efi = 0; efi < 2; ++efi)
		{
			const auto fi = e.m_fis[efi];
			if (fi != -1)
			{
				assert(0 <= fi && fi < F);
				assert(contains(m_faceEdges[fi].m_feis, ei));
			}
		}
	}
}