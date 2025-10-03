#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <define.h>
#include <map>

namespace qc
{
	class Animation {
	public:
		struct ikrule {
			int index;
			//std::string type;
			//std::string target;
			//...
		};

		struct movement {
			float v_start;
			float v_end;
		};

		struct framemovement {
			int numframes;
		};

		std::string name;
		std::string smdFile;
		size_t numframes = 1;
		std::vector<ikrule> ikrules{};
		std::vector<movement> movement{};
		framemovement framemovement = { 0 };
		float fps = 30.0f;

		Animation(const std::string& name, const std::string& smdFile, const float fps, const size_t numframes)
			: name(name), smdFile(smdFile), fps(fps), numframes(numframes) {
		}
	};

	class Sequence
	{
	public:
		struct poseparamkey {
			std::string name;
			float start = 0.0f;
			float end = 1.0f;
		};

		std::string name;
		std::vector <std::string> animName;
		std::vector<uint64_t> autolayerGuid{};
		bool loop = false;
		bool delta = false;
		float fadeIn = 0.2f;
		float fadeOut = 0.2f;

		std::string activity;
		int weightlistIdx = -1;
		int activityWeight = 0;
		std::vector<std::string> activityModifiers;
		std::vector<temp::event_t> events;
		int node;
		std::pair<int, int> transitions;
		int exitnode;
		int entrynode;
		std::vector<poseparamkey> blendkeys;
		int blendWidth = 0;
		Sequence(const std::string& name, const std::string& smdFile)
			: name(name) {
		}

		void AddEvent(const temp::event_t& e) {
			events.push_back(e);
		}

		void AddSeqNode(const int& node_idx) {
			node = node_idx;
		}

		void AddTransition(const int& from, const int& to) {
			transitions = std::pair(from, to);
		}

		void AddAnimation(const std::string& raw_name)
		{
			animName.push_back(raw_name);
		}

		// ------- not sure -------
		void AddExitNode(const int& n) {
			exitnode = n;
		}

		void AddEntryNode(const int& n) {
			entrynode = n;
		}
		// ------- not sure -------

		void AddActivityModifier(const std::string& mod) {
			activityModifiers.push_back(mod);
		}
	};

	class QCModel
	{
	public:
		std::string outPath = "";

		QCModel(const std::string& path)
			: outPath(path) {
		}

		struct bonedata {
			std::string name;
			std::string parentName;
			Vector3 pos = { 0.0f, 0.0f, 0.0f };
			RadianEuler rot = { 0.0f, 0.0f, 0.0f };
		};

		struct attachment {
			std::string name;
			std::string bone;
			Vector3 pos = { 0.0f, 0.0f, 0.0f };
			RadianEuler rot = { 0.0f, 0.0f, 0.0f };
		};

		struct poseparam {
			std::string name;
			int flags;
			float start;
			float end;
			float loop;
		};

		struct bodypart {
			std::string name;
			std::vector<std::string> models;
		};

		bool hasRrig = false;

		std::string modelname;
		Vector3 illumposition;
		Vector3 cboxmin = { 0.0, 0.0, 0.0 };
		Vector3 cboxmax = { 0.0, 0.0, 0.0 };
		Vector3 bboxmin = { 0.0, 0.0, 0.0 };
		Vector3 bboxmax = { 0.0, 0.0, 0.0 };
		std::string cdmaterials;
		std::map<int, std::string> nodes;
		std::vector<std::string> textures;
		std::vector<attachment> attachments;
		std::vector<bonedata> bones;
		std::vector<bodypart> bodyparts;
		std::vector<poseparam> poseparam;
		//std::vector<std::string> nodename;
		std::vector<std::string> animnames;
		std::vector<temp::ikchain_t> ikchain;
		std::vector<Animation> animations;
		std::map<uint64_t, std::string> autolayers;
		std::vector<Sequence> sequences;
		std::vector<std::vector<float>> weightlist;

		void AddBone(const bonedata& b)
		{
			bones.push_back(b);
		}

		void AddAnimation(const Animation& a)
		{
			for (const auto& anim : animations) {
				if (anim.name == a.name) {
					if (anim.smdFile != a.smdFile) {
						printf("Boom! \n%s\n --> %s\n --> %s\n", anim.name.c_str(), anim.smdFile.c_str(), a.smdFile.c_str());
					}
					return;
				}
			}
			animations.push_back(a);
		}

		int isWeightlistDup(const std::vector<float>& f) {
			// -1 not dup
			// else dup index
			for (int i = 0; i < weightlist.size(); i++) {
				int dupVal = 0;
				for (int j = 0; j < bones.size(); j++) {
					if ( weightlist[i][j] == f[j]){
						dupVal++;
					}
				}
				if (dupVal == bones.size()) {
					return i;
				}
			}
			return -1;
		}

		int AddWeightlist(const std::vector<float>& f) {
			int idx = weightlist.size();
			int dupIdx = isWeightlistDup(f);
			if (dupIdx == -1) {
				weightlist.push_back(f);
				return idx;
			}
			else {
				return dupIdx;
			}
		}

		void AddNode(const int idx, const std::string& nodeName) {
			nodes[idx] = nodeName;
		}

		void AddBodyPart(const bodypart& b)
		{
			bodyparts.push_back(b);
		}

		void AddAttachment(const std::string s, const std::string b, const Matrix3x4_t& m)
		{
			Vector3 pos;
			pos.x = m.m[0][3];
			pos.y = m.m[1][3];
			pos.z = m.m[2][3];

			RadianEuler rot;

			rot.y = asinf(-m.m[0][2]);

			if (cosf(rot.y) > 1e-6f) {
				rot.x = atan2f(m.m[1][2], m.m[2][2]);
				rot.z = atan2f(m.m[0][1], m.m[0][0]);
			}
			else {
				rot.x = atan2f(-m.m[2][1], m.m[1][1]);
				rot.z = 0;
			}

			rot.x = -rot.x / (2 * M_PI) * 360.0;
			rot.y = rot.y / (2 * M_PI) * 360.0;
			rot.z = -rot.z / (2 * M_PI) * 360.0;
			
			attachment att = {s, b, pos, rot};
			attachments.push_back(att);
		}

		void AddSequence(const Sequence& seq)
		{
			std::string fullseqname = seq.name + ".rseq";
			sequences.push_back(seq);
			autolayers.emplace(StringToGuid(fullseqname.c_str()), seq.name);
		}

		void SortSequences()
		{
			std::sort(sequences.begin(), sequences.end(), [](const Sequence& a, const Sequence& b) {
				return a.name < b.name;
			});
		}

		std::string GetAutoLayerName(const __int64& guid) const
		{
			auto it = autolayers.find(guid);
			if (it != autolayers.end()) {
				return it->second;
			}
			return "Error : Couldn't find an autolayer string.";
		}

		void Write() const
		{
			std::ofstream out(outPath);
			if (!out) throw std::runtime_error("Failed to write .qc file");

			if (!modelname.empty()) out << "$modelname \"" << modelname << "\"\n\n";

			for (const auto& b : bodyparts) {
				out << "$bodygroup \"" << b.name << "\"\n"; 
				out << "{\n";
				for (const auto& model : b.models) {
					if (!model.empty()) {
						out << "\tstudio \"" << model << ".smd\"\n";
					}
					else {
						out << "\tblank\n";
					}
				}
				out << "}\n";
			}
			if (!bodyparts.empty()) out << "\n";

			if (hasRrig) {
				// illumposition
				out << "$illumposition "
					<< std::fixed << std::setprecision(6)
					<< illumposition.x << " " << illumposition.y << " " << illumposition.z << "\n";

				// cbox
				out << "$cbox "
					<< cboxmin.x << " " << cboxmin.y << " " << cboxmin.z << " "
					<< cboxmax.x << " " << cboxmax.y << " " << cboxmax.z << "\n";

				// bbox
				out << "$bbox "
					<< bboxmin.x << " " << bboxmin.y << " " << bboxmin.z << " "
					<< bboxmax.x << " " << bboxmax.y << " " << bboxmax.z << "\n\n";


				// TODO: hboxset bboxes
				 
				// texutures
				if(!cdmaterials.empty()) out << "$cdmaterials \"" << cdmaterials << "\"\n\n";

				// attachments
				for(const auto& att : attachments) {
					out << "$attachment \"" << att.name << "\" \"" << att.bone << "\" "
						<< att.pos.x << " " << att.pos.y << " " << att.pos.z << " rotate "
						<< att.rot.x << " " << att.rot.y << " " << att.rot.z << "\n";
				}
				if (!attachments.empty()) out << "\n";

				// poseparam
				for (const auto& pp : poseparam) {
					out << "$poseparameter \"" << pp.name << "\" " << pp.flags << " "
						<< pp.start << " " << pp.end;
					if (pp.loop > 0.0f)
						out << " loop " << pp.loop; // not sure about this
					out << "\n";
				}
				if (!poseparam.empty()) out << "\n";

				if (!bones.empty()) out << "$unlockdefinebones" << "\n";
				for (auto& b : bones) {
					out << "$definebone \"" << b.name << "\" \"" << b.parentName << "\" "
						<< std::setprecision(16)
						<< b.pos.x << " " << b.pos.y << " " << b.pos.z << " "
						<< b.rot.x << " " << b.rot.y << " " << b.rot.z
						<< " 0 0 0 0 0 0"
						<< std::setprecision(6) << "\n";
				}
				out << "\n";
				for (auto& b : bones) {
					out << "$bonemerge \"" << b.name << "\"\n";
				}
				out << "\n";

				// weightlist
				for (int i = 0; i < weightlist.size(); i++) {
					out << "$weightlist weight_" << i << " {\n";
					if (weightlist[i].size() != bones.size()) {
						out << "// warning: size of weightlist(" << weightlist[i].size() << ") and bones(" << bones.size() << ") are not the same.\n";
					}
					for (int j = 0; j < bones.size(); j++) {
						out << "\t" << bones[j].name << " " << weightlist[i][j] << "\n";
					}
					out << "}\n\n";
				}
			}
			
			// animation
			if (!animations.empty()) out << "$sectionframes 63 120\n\n";
			for (const auto& anim : animations) {
				out << "$animation " << anim.name << " \"" << anim.smdFile << "\" {\n";
				out << "\tfps " << anim.fps << "\n";

				out << "}\n\n";
			}

			// sequences
			for (const auto& seq : sequences)
			{
				out << "$sequence \"" << seq.name << "\" {\n";

				for (const auto& animName : seq.animName) {
					if (animName.empty()) continue;
					out << "\t\"" << animName << "\"\n";
				}

				out << "\n";

				if (seq.fadeIn >= 0.0f) out << "\tfadein " << seq.fadeIn << "\n";
				if (seq.fadeOut >= 0.0f) out << "\tfadeout " << seq.fadeOut << "\n";
				if (seq.loop) out << "\tloop\n";
				if (seq.delta) out << "\tdelta\n";

				if (!seq.activity.empty())
					out << "\tactivity " << seq.activity << " " << seq.activityWeight << "\n";

				for (const auto& mod : seq.activityModifiers)
					out << "\tactivitymodifier \"" << mod << "\"\n";

				if (seq.blendWidth > 1)
					out << "\tblendwidth " << seq.blendWidth << "\n";

				for (auto& b : seq.blendkeys) {
					out << "\tblend " << b.name << " " << b.start << " " << b.end << "\n";
				}

				for (auto& guid : seq.autolayerGuid) {
					std::string layerName = GetAutoLayerName(guid);
					out << "\taddlayer \"" << layerName << "\"" << "\n";
				}

				// unfinished
				for (auto& ikrule : animations[0].ikrules)
					out << "\t//ikrule " << ikrule.index << "\n";

				for (auto& mm : animations[0].movement)
					out << "\t//movement " << mm.v_start << " " << mm.v_end << "\n";

				if (animations[0].framemovement.numframes > 0)
					out << "\t//movement framemovement" << animations[0].framemovement.numframes << "\n";
				// unfinished

				if (seq.weightlistIdx != -1) out << "\tweightlist weight_" << seq.weightlistIdx << "\n";

				// Events
				for (const auto& e : seq.events)
					out << "\t{ event " << e.options << " " << e.frame << " \"" << e.eventName << "\" }\n";

				// Nodes and transitions
				if (seq.node) {
					auto it = nodes.find(seq.node); 
					out << "\tnode ";
					if (it != nodes.end()) {
						 out << it->second << "\n";
					}
					else {
						out << "NODE_" << seq.node << "\n";
					}
				}

				if (seq.transitions.first != 0 && seq.transitions.second != 0) {
					auto it_f = nodes.find(seq.transitions.first);
					auto it_s = nodes.find(seq.transitions.second);
					out << "\ttransition ";

					// from
					if (it_f != nodes.end()) {
						out << it_f->second;
					}
					else {
						out << "NODE_" << seq.transitions.first;
					}

					// to
					out << " ";
					if (it_s != nodes.end()) {
						out << it_s->second;
					}
					else {
						out << "NODE_" << seq.transitions.second;
					}
					out << "\n";
				}

				if (seq.exitnode) {
					auto it = nodes.find(seq.exitnode);
					out << "\t//transition 0 ";
					if (it != nodes.end()) {
						out << it->second << "\n";
					}
					else {
						out << "NODE_" << seq.exitnode << "\n";
					}
				}

				if (seq.entrynode) {
					auto it = nodes.find(seq.entrynode);
					out << "\t//transition ";
					if (it != nodes.end()) {
						out << it->second << " 0\n";
					}
					else {
						out << "NODE_" << seq.entrynode << " 0\n";
					}
				}

				out << "}\n\n";
			}
		}
	};
}


std::string getName(std::string anim_name);
//
void readRrig_v10(std::string path, qc::QCModel& modelOut);
void readRmdl_v10(std::string path, qc::QCModel& modelOut);
void readRseq_v7(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut);
//
void readRrig_v13(std::string path, qc::QCModel& modelOut);
void readRmdl_v121(std::string path, qc::QCModel& modelOut);
//void readRseq_v7(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut);
//
void readRrig_v16(std::string path, qc::QCModel& modelOut);
void readRmdl_v16(std::string path, qc::QCModel& modelOut);
void readRseq_v12(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut);
//