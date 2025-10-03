#include <define.h>
#include <mdl/studio.h>
#include <mdl/qc.h>
#include <string>
#include <sstream>

std::string getName(std::string anim_name) {
	std::string name;
	std::replace(anim_name.begin(), anim_name.end(), '\\', '/');
	size_t slash_pos = anim_name.find_last_of('/');
	name = (slash_pos != std::string::npos) ? anim_name.substr(slash_pos + 1) : anim_name;
	return name;
}

void readRrig_v10(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rrig_stream(path, std::ios::binary);

	char* buffer = new char[inputFileSize];
	rrig_stream.read(buffer, inputFileSize);
	rrig_stream.close();

	auto* rrigHdr = reinterpret_cast<r5::v10::studiohdr_t*>(buffer);
	auto* poseparamDesc = PTR_FROM_IDX(r5::v10::mstudioposeparamdesc_t, rrigHdr, rrigHdr->localposeparamindex);
	auto* raw_bone = PTR_FROM_IDX(r5::v10::mstudiobone_t, buffer, rrigHdr->boneindex);
	auto* nodename_index = PTR_FROM_IDX(int, rrigHdr, rrigHdr->localnodenameindex);

	modelOut.modelname = STRING_FROM_IDX(buffer, rrigHdr->sznameindex);
	modelOut.illumposition = rrigHdr->illumposition;
	modelOut.cboxmin = rrigHdr->hull_min;
	modelOut.cboxmax = rrigHdr->hull_max;
	modelOut.bboxmin = rrigHdr->view_bbmin;
	modelOut.bboxmax = rrigHdr->view_bbmax;

	for (int i = 0; i < rrigHdr->numbones; i++) {
		std::string name = STRING_FROM_IDX(&raw_bone[i], raw_bone[i].sznameindex);
		int parent_index = raw_bone[i].parent;
		std::string parentname;
		if (parent_index >= 0) {
			parentname = STRING_FROM_IDX(&raw_bone[raw_bone[i].parent], raw_bone[raw_bone[i].parent].sznameindex);
		}
		qc::QCModel::bonedata bonedata;
		bonedata.name = name;
		bonedata.parentName = parent_index == -1 ? "" : parentname;
		bonedata.pos = raw_bone[i].pos;
		bonedata.rot = raw_bone[i].rot;
		modelOut.AddBone(bonedata);
	}

	for (int i = 0; i < rrigHdr->numlocalposeparameters; i++) {
		std::string poseparam_name = STRING_FROM_IDX(&poseparamDesc[i], poseparamDesc[i].sznameindex);
		modelOut.poseparam.push_back({ poseparam_name,poseparamDesc[i].flags, poseparamDesc[i].start, poseparamDesc[i].end, poseparamDesc[i].loop }); // flags?
	}

	for (int i = 0; i < rrigHdr->numlocalnodes; i++) {
		std::string node_name = STRING_FROM_IDX(rrigHdr, nodename_index[i]);
		modelOut.AddNode(i + 1, node_name);
	}
	r5::v10::mstudioikchain_t* ikchainDesc = PTR_FROM_IDX(r5::v10::mstudioikchain_t, rrigHdr, rrigHdr->ikchainindex);
	for (int i = 0; i < rrigHdr->numikchains; i++) {
		int* iklink_index = PTR_FROM_IDX(int, &ikchainDesc[i], ikchainDesc[i].linkindex);
		std::string chain_name = STRING_FROM_IDX(ikchainDesc, ikchainDesc[i].sznameindex);
		std::string bone_name = modelOut.bones[iklink_index[2]].name;
		modelOut.ikchain.push_back({ chain_name, bone_name, ikchainDesc[i].unk });
	}
	delete[] buffer;
}

void readRrig_v13(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rrig_stream(path, std::ios::binary);

	char* buffer = new char[inputFileSize];
	rrig_stream.read(buffer, inputFileSize);
	rrig_stream.close();

	auto* rrigHdr = reinterpret_cast<r5::v13::studiohdr_t*>(buffer);
	auto* poseparamDesc = PTR_FROM_IDX(r5::v10::mstudioposeparamdesc_t, rrigHdr, rrigHdr->localposeparamindex);
	auto* raw_bone = PTR_FROM_IDX(r5::v121::mstudiobone_t, buffer, rrigHdr->boneindex);
	auto* nodename_index = PTR_FROM_IDX(int, rrigHdr, rrigHdr->localnodenameindex);

	modelOut.modelname = STRING_FROM_IDX(buffer, rrigHdr->sznameindex);
	modelOut.illumposition = rrigHdr->illumposition;
	modelOut.cboxmin = rrigHdr->hull_min;
	modelOut.cboxmax = rrigHdr->hull_max;
	modelOut.bboxmin = rrigHdr->view_bbmin;
	modelOut.bboxmax = rrigHdr->view_bbmax;

	for (int i = 0; i < rrigHdr->numbones; i++) {
		std::string name = STRING_FROM_IDX(&raw_bone[i], raw_bone[i].sznameindex);
		int parent_index = raw_bone[i].parent;
		std::string parentname;
		if (parent_index >= 0) {
			parentname = STRING_FROM_IDX(&raw_bone[raw_bone[i].parent], raw_bone[raw_bone[i].parent].sznameindex);
		}
		qc::QCModel::bonedata bonedata;
		bonedata.name = name;
		bonedata.parentName = parent_index == -1 ? "" : parentname;
		bonedata.pos = raw_bone[i].pos;
		bonedata.rot = raw_bone[i].rot;
		modelOut.AddBone(bonedata);
	}

	for (int i = 0; i < rrigHdr->numlocalposeparameters; i++) {
		std::string poseparam_name = STRING_FROM_IDX(&poseparamDesc[i], poseparamDesc[i].sznameindex);
		modelOut.poseparam.push_back({ poseparam_name,poseparamDesc[i].flags, poseparamDesc[i].start, poseparamDesc[i].end, poseparamDesc[i].loop }); // flags?
	}

	for (int i = 0; i < rrigHdr->numlocalnodes; i++) {
		std::string node_name = STRING_FROM_IDX(rrigHdr, nodename_index[i]);
		modelOut.AddNode(i + 1, node_name);
	}
	r5::v10::mstudioikchain_t* ikchainDesc = PTR_FROM_IDX(r5::v10::mstudioikchain_t, rrigHdr, rrigHdr->ikchainindex);
	for (int i = 0; i < rrigHdr->numikchains; i++) {
		int* iklink_index = PTR_FROM_IDX(int, &ikchainDesc[i], ikchainDesc[i].linkindex);
		std::string chain_name = STRING_FROM_IDX(ikchainDesc, ikchainDesc[i].sznameindex);
		std::string bone_name = modelOut.bones[iklink_index[2]].name;
		modelOut.ikchain.push_back({ chain_name, bone_name, ikchainDesc[i].unk });
	}
	delete[] buffer;
}

void readRrig_v16(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rrig_stream(path, std::ios::binary);

	char* buffer = new char[inputFileSize];
	rrig_stream.read(buffer, inputFileSize);
	rrig_stream.close();

	r5::v16::studiohdr_t* rrigHdr = reinterpret_cast<r5::v16::studiohdr_t*>(buffer);
	r5::v16::mstudioposeparamdesc_t* poseparamDesc = PTR_FROM_IDX(r5::v16::mstudioposeparamdesc_t, rrigHdr, rrigHdr->localposeparamindex);
	r5::v16::mstudiobone_t* raw_bone = PTR_FROM_IDX(r5::v16::mstudiobone_t, buffer, rrigHdr->boneindex);
	auto* linearboneindex = PTR_FROM_IDX(r5::v16::linearboneindex_t, buffer, rrigHdr->linearboneindex);
	auto* parentbone = PTR_FROM_IDX(short, linearboneindex, linearboneindex->parentindex);
	auto* studioPos = PTR_FROM_IDX(Vector3, linearboneindex, linearboneindex->posindex);
	auto* studioRot = PTR_FROM_IDX(RadianEuler, linearboneindex, linearboneindex->rotindex);
	auto* nodename_index = PTR_FROM_IDX(short, rrigHdr, rrigHdr->localnodenameindex);

	modelOut.modelname = STRING_FROM_IDX(buffer, rrigHdr->sznameindex);
	modelOut.illumposition = rrigHdr->illumposition;
	modelOut.cboxmin = rrigHdr->hull_min;
	modelOut.cboxmax = rrigHdr->hull_max;
	modelOut.bboxmin = rrigHdr->view_bbmin;
	modelOut.bboxmax = rrigHdr->view_bbmax;

	for (int i = 0; i < rrigHdr->numbones; i++) {
		short parent_index = parentbone[i];
		std::string name = STRING_FROM_IDX(&raw_bone[i], raw_bone[i].sznameindex);
		std::string parentname = STRING_FROM_IDX(&raw_bone[parent_index], raw_bone[parent_index].sznameindex);

		qc::QCModel::bonedata bonedata;
		bonedata.name = name;
		bonedata.parentName = parent_index == -1 ? "" : parentname;
		bonedata.pos = studioPos[i];
		bonedata.rot = studioRot[i];
		modelOut.AddBone(bonedata);
	}

	for (int i = 0; i < rrigHdr->numlocalposeparameters; i++) {
		std::string poseparam_name = STRING_FROM_IDX(&poseparamDesc[i], poseparamDesc[i].sznameindex);
		modelOut.poseparam.push_back({ poseparam_name,poseparamDesc[i].flags, poseparamDesc[i].start, poseparamDesc[i].end, poseparamDesc[i].loop });
	}

	for (int i = 0; i < rrigHdr->numlocalnodes; i++) {
		std::string node_name = STRING_FROM_IDX(nodename_index, nodename_index[i]);
		modelOut.AddNode(i + 1, node_name);
	}
	r5::v16::mstudioikchain_t* ikchainDesc = PTR_FROM_IDX(r5::v16::mstudioikchain_t, rrigHdr, rrigHdr->ikchainindex);
	for (int i = 0; i < rrigHdr->numikchains; i++) {
		int* iklink_index = PTR_FROM_IDX(int, &ikchainDesc[i], ikchainDesc[i].linkindex);
		std::string chain_name = STRING_FROM_IDX(ikchainDesc, ikchainDesc[i].sznameindex);
		std::string bone_name = modelOut.bones[iklink_index[2 * 4]].name;
		modelOut.ikchain.push_back({ chain_name, bone_name, ikchainDesc[i].unk });
	}
	delete[] buffer;
}

void readRseq_v7(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut) {
	int num_frames = 0;
	float fps = 0;

	for (const auto& file : paths) {
		size_t inputFileSize = std::filesystem::file_size(file);
		std::ifstream rseq_stream(file, std::ios::binary);
		std::vector<char> buffer(inputFileSize);
		rseq_stream.read(buffer.data(), inputFileSize);

		// bad size
		if (inputFileSize <= sizeof(r5::v16::mstudioseqdesc_t)) {
			printf("[!] Skipping %s (%zu byte)\n", getName(file).c_str(), inputFileSize);
			continue;
		}

		auto* rseqDesc = reinterpret_cast<r5::v10::mstudioseqdesc_t*>(buffer.data());

		std::string seq_name_raw = STRING_FROM_IDX(buffer.data(), rseqDesc->szlabelindex);
		std::string seq_name = seq_name_raw.substr(0, seq_name_raw.rfind('.'));
		std::string seq_act = STRING_FROM_IDX(buffer.data(), rseqDesc->szactivitynameindex);

		qc::Sequence seq(seq_name, "");

		auto* autolayers = PTR_FROM_IDX(r5::v10::mstudioautolayer_t, buffer.data(), rseqDesc->autolayerindex); //didn't test
		for (int i = 0; i < rseqDesc->numautolayers; i++) {
			__int64 guid = autolayers[i].guidSequence;
			seq.autolayerGuid.push_back(guid);
		}

		int* blend_idx = PTR_FROM_IDX(int, buffer.data(), rseqDesc->animindexindex);
		uint32_t blendwidth = std::max(rseqDesc->groupsize[0], rseqDesc->groupsize[1]);

		seq.blendWidth = blendwidth;

		for (int i = 0; i < rseqDesc->numblends; i++) {
			auto* animDesc = PTR_FROM_IDX(r5::v10::mstudioanimdesc_t, buffer.data(), blend_idx[i]);

			num_frames = std::max(num_frames, animDesc->numframes - 1);
			fps = std::max(fps, animDesc->fps);

			std::string anim_name = STRING_FROM_IDX(animDesc, animDesc->sznameindex);
			std::string raw_name = getName(anim_name);
			std::string animFile;

			std::string n = anim_name;
			std::filesystem::path p(n);
			std::filesystem::path s(seq_name);
			std::filesystem::path rp = std::filesystem::relative(file, in_dir);
			std::string f = s.begin()->string();
			raw_name = f + "_" + raw_name;

			if (s.has_parent_path()) {
				animFile = rp.parent_path().string() + "/" + p.stem().string();
			}
			else {
				animFile = "anims/" + p.stem().string();
			}
			animFile += ".smd";

			qc::Animation rawAnim(raw_name, animFile, animDesc->fps, animDesc->numframes);

			auto* ikrule = PTR_FROM_IDX(r5::v10::mstudioikrule_t, animDesc, animDesc->ikruleindex);
			for (int j = 0; j < animDesc->numikrules; j++) {
				rawAnim.ikrules.push_back({ ikrule[j].index });
			}

			if (animDesc->movementindex) {
				auto* movement = PTR_FROM_IDX(r5::v10::mstudiomovement_t, animDesc, animDesc->movementindex);
				for (int j = 0; j < animDesc->nummovements; j++) {
					rawAnim.movement.push_back({ movement->v_start, movement->v_end });
				}
			}

			if (animDesc->framemovementindex) {
				auto* movement = PTR_FROM_IDX(r5::v10::mstudioframemovement_t, animDesc, animDesc->framemovementindex);
				rawAnim.framemovement = { movement->sectionFrames };
			}

			seq.AddAnimation(raw_name);
			modelOut.AddAnimation(rawAnim);
		}
		if (rseqDesc->flags & 0x1) seq.loop = true;
		if (rseqDesc->flags & 0x4) seq.delta = true;

		seq.fadeIn = rseqDesc->fadeintime;
		seq.fadeOut = rseqDesc->fadeouttime;

		if (!seq_act.empty()) {
			seq.activity = seq_act;
			seq.activityWeight = rseqDesc->actweight;
		}

		auto* activityMods = PTR_FROM_IDX(r5::v10::mstudioactivitymodifier_t, buffer.data(), rseqDesc->activitymodifierindex);
		for (int i = 0; i < rseqDesc->numactivitymodifiers; i++) {
			std::string mod = STRING_FROM_IDX(&activityMods[i], activityMods[i].sznameindex);
			if (activityMods[i].negate) mod += "_1";
			seq.AddActivityModifier(mod);
		}

		if (blendwidth > 1) {
			for (int i = 0; i < 2; i++) {
				if (rseqDesc->paramindex[i] != -1) {
					std::string paramName;
					if (modelOut.hasRrig) {
						paramName = modelOut.poseparam.at(rseqDesc->paramindex[i]).name;
					}
					else {
						paramName = "POSEPARAM_" + std::to_string(rseqDesc->paramindex[i]);
					}
					seq.blendkeys.push_back({ paramName,rseqDesc->paramstart[i],rseqDesc->paramend[i] });
				}
			}
		}

		// Events
		auto* events = PTR_FROM_IDX(r5::v10::mstudioevent_t, buffer.data(), rseqDesc->eventindex);
		for (int i = 0; i < rseqDesc->numevents; i++) {
			std::string eventName = STRING_FROM_IDX(&events[i], events[i].szeventindex);
			std::string eventOption = events[i].options;
			int frame = static_cast<int>(events[i].cycle * num_frames);

			std::stringstream ss;
			ss << eventName << " " << frame << " \"" << eventOption << "\"";
			temp::event_t event(frame, eventName, eventOption);
			seq.AddEvent(event);
		}

		// Nodes / Transitions
		short entrynode = rseqDesc->localentrynode;
		short exitnode = rseqDesc->localexitnode;
		//bool hasNodeEntryData = modelOut.nodename.size() > (entrynode - 1);
		//bool hasNodeExitData = modelOut.nodename.size() > (exitnode - 1);

		//if ((entrynode != exitnode) && (entrynode != 0) && (exitnode != 0)) {
		//	std::string entryNode, exitNode;
		//	if (hasNodeEntryData && hasNodeExitData) {
		//		entryNode = (entrynode == 0) ? "" : modelOut.nodename.at(entrynode - 1);
		//		exitNode = (exitnode == 0) ? "" : modelOut.nodename.at(exitnode - 1);
		//	}
		//	else {
		//		entryNode = "ENTRYNODE_" + std::to_string(entrynode);
		//		exitNode = "EXITNODE_" + std::to_string(exitnode);
		//	}
		//	seq.AddTransition(entryNode, exitNode);
		//}
		//else if ((entrynode == exitnode) && (entrynode != 0)) {
		//	std::string nodeName;
		//	if (hasNodeEntryData) {
		//		nodeName = modelOut.nodename.at(entrynode - 1);
		//	}
		//	else {
		//		nodeName = "NODE_" + std::to_string(entrynode);
		//	}
		//	seq.AddNode(nodeName);
		//}
		//else if ((entrynode != exitnode) && (entrynode == 0)) {
		//	std::string nodeName;
		//	if (hasNodeExitData) {
		//		nodeName = modelOut.nodename.at(exitnode - 1);
		//	}
		//	else {
		//		nodeName = "NODE_" + std::to_string(exitnode);
		//	}
		//	seq.AddEntryNode(nodeName);
		//}
		//else if ((entrynode != exitnode) && (exitnode == 0)) {
		//	std::string nodeName;
		//	if (hasNodeEntryData) {
		//		nodeName = modelOut.nodename.at(entrynode - 1);
		//	}
		//	else {
		//		nodeName = "NODE_" + std::to_string(entrynode);
		//	}
		//	seq.AddExitNode(nodeName);
		//}

		modelOut.AddSequence(seq);

		num_frames = 0;
		fps = 0;
	}
}

void readRseq_v12(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut) {
	int num_frames = 0;
	float fps = 0;

	for (const auto& file : paths) {
		size_t inputFileSize = std::filesystem::file_size(file);
		std::ifstream rseq_stream(file, std::ios::binary);
		std::vector<char> buffer(inputFileSize);
		rseq_stream.read(buffer.data(), inputFileSize);

		// bad size
		if (inputFileSize <= sizeof(r5::v16::mstudioseqdesc_t)) {
			printf("[!] Skipping %s (%zu byte)\n", getName(file).c_str(), inputFileSize);
			continue;
		}

		auto* rseqDesc = reinterpret_cast<r5::v16::mstudioseqdesc_t*>(buffer.data());

		std::string seq_name_raw = STRING_FROM_IDX(buffer.data(), rseqDesc->szlabelindex);
		std::string seq_name = seq_name_raw.substr(0, seq_name_raw.rfind('.'));
		std::string seq_act = STRING_FROM_IDX(buffer.data(), rseqDesc->szactivitynameindex);

		qc::Sequence seq(seq_name, "");
		int numbones = (rseqDesc->activitymodifierindex - rseqDesc->weightlistindex) / 4;

		if (rseqDesc->weightlistindex >= rseqDesc->autolayerindex) {
			std::vector<float> weightlist{};
			int numHasValue = 0;
			float* weight = PTR_FROM_IDX(float, buffer.data(), rseqDesc->weightlistindex);
			for (int i = 0; i < numbones; i++) {
				weightlist.push_back(weight[i]);
				if (weight[i] != 1) {
					numHasValue++;
				}
			}
			if (numHasValue != numbones) {
				seq.weightlistIdx = modelOut.AddWeightlist(weightlist);
			}
		}


		auto* autolayers = PTR_FROM_IDX(r5::v10::mstudioautolayer_t, buffer.data(), rseqDesc->autolayerindex);
		for (int i = 0; i < rseqDesc->numautolayers; i++) {
			auto guid = autolayers[i].guidSequence;
			seq.autolayerGuid.push_back(guid);
		}

		auto* blend_idx = PTR_FROM_IDX(short, buffer.data(), rseqDesc->animindexindex);
		uint32_t blendwidth = std::max(rseqDesc->groupsize[0], rseqDesc->groupsize[1]);

		seq.blendWidth = blendwidth;

		for (int i = 0; i < rseqDesc->numblends; i++) {
			auto* animDesc = PTR_FROM_IDX(r5::v16::mstudioanimdesc_t, buffer.data(), blend_idx[i]);

			num_frames = std::max(num_frames, animDesc->numframes - 1);
			fps = std::max(fps, animDesc->fps);

			std::string anim_name;
			
			// zeroanim
			int base = (char*)animDesc - buffer.data();
			if (base + animDesc->sznameindex >= inputFileSize) {
				anim_name = "zeroanim";
			}
			else {
				anim_name = STRING_FROM_IDX(animDesc, animDesc->sznameindex);
			}

			std::string raw_name = getName(anim_name);
			std::string animFile;

			std::string n = anim_name;
			std::filesystem::path p(n);
			std::filesystem::path s(seq_name);
			std::filesystem::path rp = std::filesystem::relative(file, in_dir);
			std::string f = s.begin()->string();
			raw_name = f + "_" + raw_name;

			if (s.has_parent_path()) {
				animFile = rp.parent_path().string() + "/" + p.stem().string();
			}
			else {
				animFile = "anims/" + p.stem().string();
			}
			animFile += ".smd";

			qc::Animation rawAnim(raw_name, animFile, animDesc->fps, animDesc->numframes);

			//auto* ikrule = PTR_FROM_IDX(r5::v12::mstudioikrule_t, animDesc, animDesc->ikruleindex);
			for (int j = 0; j < animDesc->numikrules; j++) {
				rawAnim.ikrules.push_back({ 0 }); //ikrule[j].index
			}

			if (animDesc->framemovementindex) {
				auto* movement = PTR_FROM_IDX(r5::v10::mstudioframemovement_t, animDesc, animDesc->framemovementindex);
				rawAnim.framemovement = { movement->sectionFrames };
			}

			seq.AddAnimation(raw_name);
			modelOut.AddAnimation(rawAnim);

		}
		if (rseqDesc->flags & 0x1) seq.loop = true;
		if (rseqDesc->flags & 0x4) seq.delta = true;

		seq.fadeIn = rseqDesc->fadeintime;
		seq.fadeOut = rseqDesc->fadeouttime;

		if (!seq_act.empty()) {
			seq.activity = seq_act;
			seq.activityWeight = rseqDesc->actweight;
		}

		auto* activityMods = PTR_FROM_IDX(r5::v16::mstudioactivitymodifier_t, buffer.data(), rseqDesc->activitymodifierindex);
		for (int i = 0; i < rseqDesc->numactivitymodifiers; i++) {
			std::string mod = STRING_FROM_IDX(&activityMods[i], activityMods[i].sznameindex);
			if (activityMods[i].negate) mod += "_1";
			seq.AddActivityModifier(mod);
		}

		if (blendwidth > 1) {
			for (int i = 0; i < 2; i++) {
				if (rseqDesc->paramindex[i] != -1) {
					std::string paramName;
					if (modelOut.poseparam.size() > i) {
						paramName = modelOut.poseparam.at(rseqDesc->paramindex[i]).name;
					}
					else {
						paramName = "POSEPARAM_" + std::to_string(rseqDesc->paramindex[i]);
					}
					seq.blendkeys.push_back({ paramName,rseqDesc->paramstart[i],rseqDesc->paramend[i] });
				}
			}
		}

		// Events
		auto* events = PTR_FROM_IDX(r5::v16::mstudioevent_t, buffer.data(), rseqDesc->eventindex);
		for (int i = 0; i < rseqDesc->numevents; i++) {
			std::string eventName = STRING_FROM_IDX(&events[i], events[i].szeventindex);
			std::string eventOption = STRING_FROM_IDX(&events[i], events[i].szoptionsindex); //events[i].options;
			int frame = static_cast<int>(events[i].cycle * num_frames);

			std::stringstream ss;
			ss << eventName << " " << frame << " \"" << eventOption << "\"";
			temp::event_t event(frame, eventName, eventOption);
			seq.AddEvent(event);
		}

		// Nodes / Transitions
		short entrynode = rseqDesc->localentrynode;
		short exitnode = rseqDesc->localexitnode;

		if ((entrynode != exitnode) && (entrynode != 0) && (exitnode != 0)) {
			seq.AddTransition(entrynode, exitnode);
		}
		else if ((entrynode == exitnode) && (entrynode != 0)) {
			seq.AddSeqNode(entrynode);
		}
		else if ((entrynode != exitnode) && (entrynode == 0)) {
			seq.AddEntryNode(exitnode);
		}
		else if ((entrynode != exitnode) && (exitnode == 0)) {
			seq.AddExitNode(entrynode);
		}
		 
		//if ((entrynode != exitnode) && (entrynode != 0) && (exitnode != 0)) {
		//	std::string entryNode, exitNode;
		//	if (hasNodeEntryData && hasNodeExitData) {
		//		entryNode = (entrynode == 0) ? "" : modelOut.nodename.at(entrynode - 1);
		//		exitNode = (exitnode == 0) ? "" : modelOut.nodename.at(exitnode - 1);
		//	}
		//	else {
		//		entryNode = "ENTRYNODE_" + std::to_string(entrynode);
		//		exitNode = "EXITNODE_" + std::to_string(exitnode);
		//	}
		//	seq.AddTransition(entryNode, exitNode);
		//}
		//else if ((entrynode == exitnode) && (entrynode != 0)) {
		//	std::string nodeName;
		//	if (hasNodeEntryData) {
		//		nodeName = modelOut.nodename.at(entrynode - 1);
		//	}
		//	else {
		//		nodeName = "NODE_" + std::to_string(entrynode);
		//	}
		//	seq.AddNode(nodeName);
		//}
		//else if ((entrynode != exitnode) && (entrynode == 0)) {
		//	std::string nodeName;
		//	if (hasNodeExitData) {
		//		nodeName = modelOut.nodename.at(exitnode - 1);
		//	}
		//	else {
		//		nodeName = "NODE_" + std::to_string(exitnode);
		//	}
		//	seq.AddEntryNode(nodeName);
		//}
		//else if ((entrynode != exitnode) && (exitnode == 0)) {
		//	std::string nodeName;
		//	if (hasNodeEntryData) {
		//		nodeName = modelOut.nodename.at(entrynode - 1);
		//	}
		//	else {
		//		nodeName = "NODE_" + std::to_string(entrynode);
		//	}
		//	seq.AddExitNode(nodeName);
		//}

		modelOut.AddSequence(seq);

		num_frames = 0;
		fps = 0;
	}
}

void readRmdl_v10(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rmdl_stream(path, std::ios::binary);

	char* buffer = new char[inputFileSize];
	rmdl_stream.read(buffer, inputFileSize);
	rmdl_stream.close();

	auto* rmdlHdr = reinterpret_cast<r5::v10::studiohdr_t*>(buffer);
	modelOut.modelname = STRING_FROM_IDX(rmdlHdr, rmdlHdr->sznameindex);

	auto* bones = PTR_FROM_IDX(r5::v10::mstudiobone_t, rmdlHdr, rmdlHdr->boneindex);
	auto* attachments = PTR_FROM_IDX(r5::v10::mstudioattachment_t, rmdlHdr, rmdlHdr->localattachmentindex);
	for (int i = 0; i < rmdlHdr->numlocalattachments; i++) {
		std::string name = STRING_FROM_IDX(&attachments[i], attachments[i].sznameindex);
		std::string bonename = STRING_FROM_IDX(&bones[attachments[i].localbone], bones[attachments[i].localbone].sznameindex);
		modelOut.AddAttachment(name, bonename, attachments[i].localmatrix);
	}

	//auto* textures = PTR_FROM_IDX(r5::v10::mstudiotexture_t, rmdlHdr, rmdlHdr->textureindex);

	auto* bodyparts = PTR_FROM_IDX(r5::v10::mstudiobodyparts_t, rmdlHdr, rmdlHdr->bodypartindex);
	for (int i = 0; i < rmdlHdr->numbodyparts; i++) {
		qc::QCModel::bodypart bodypart{};
		std::string name = STRING_FROM_IDX(&bodyparts[i], bodyparts[i].sznameindex);
		auto* modeldesc = PTR_FROM_IDX(r5::v10::mstudiomodel_t, &bodyparts[i], bodyparts[i].modelindex);
		int modelnameiter = 0;
		for (int j = 0; j < bodyparts[i].nummodels; j++) {
			std::string modelname = "";
			if (modeldesc[j].nummeshes) {
				modelname = name + "_" + std::to_string(modelnameiter);
				modelnameiter++;
			}
			bodypart.models.push_back(modelname);
		}
		bodypart.name = name;
		modelOut.AddBodyPart(bodypart);
	}
	delete[] buffer;
}

void readRmdl_v121(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rmdl_stream(path, std::ios::binary);

	char* buffer = new char[inputFileSize];
	rmdl_stream.read(buffer, inputFileSize);
	rmdl_stream.close();

	auto* rmdlHdr = reinterpret_cast<r5::v121::studiohdr_t*>(buffer);
	modelOut.modelname = STRING_FROM_IDX(rmdlHdr, rmdlHdr->sznameindex);

	auto* bones = PTR_FROM_IDX(r5::v121::mstudiobone_t, rmdlHdr, rmdlHdr->boneindex);
	auto* attachments = PTR_FROM_IDX(r5::v10::mstudioattachment_t, rmdlHdr, rmdlHdr->localattachmentindex);
	for (int i = 0; i < rmdlHdr->numlocalattachments; i++) {
		std::string name = STRING_FROM_IDX(&attachments[i], attachments[i].sznameindex);
		std::string bonename = STRING_FROM_IDX(&bones[attachments[i].localbone], bones[attachments[i].localbone].sznameindex);
		modelOut.AddAttachment(name, bonename, attachments[i].localmatrix);
	}

	auto* bodyparts = PTR_FROM_IDX(r5::v10::mstudiobodyparts_t, rmdlHdr, rmdlHdr->bodypartindex);
	for (int i = 0; i < rmdlHdr->numbodyparts; i++) {
		qc::QCModel::bodypart bodypart{};
		std::string name = STRING_FROM_IDX(&bodyparts[i], bodyparts[i].sznameindex);
		auto* modeldesc = PTR_FROM_IDX(r5::v121::mstudiomodel_t, &bodyparts[i], bodyparts[i].modelindex);
		int modelnameiter = 0;
		for (int j = 0; j < bodyparts[i].nummodels; j++) {
			std::string modelname = "";
			if (modeldesc[j].nummeshes) {
				modelname = name + "_" + std::to_string(modelnameiter);
				modelnameiter++;
			}
			bodypart.models.push_back(modelname);
		}
		bodypart.name = name;
		modelOut.AddBodyPart(bodypart);
	}
	delete[] buffer;
}

void readRmdl_v16(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rmdl_stream(path, std::ios::binary);

	char* buffer = new char[inputFileSize];
	rmdl_stream.read(buffer, inputFileSize);
	rmdl_stream.close();

	r5::v16::studiohdr_t* rmdlHdr = reinterpret_cast<r5::v16::studiohdr_t*>(buffer);
	modelOut.modelname = STRING_FROM_IDX(rmdlHdr, rmdlHdr->sznameindex);

	auto* bones = PTR_FROM_IDX(r5::v16::mstudiobone_t, rmdlHdr, rmdlHdr->boneindex);
	auto* attachments = PTR_FROM_IDX(r5::v16::mstudioattachment_t, rmdlHdr, rmdlHdr->localattachmentindex);
	for (int i = 0; i < rmdlHdr->numlocalattachments; i++) {
		std::string name = STRING_FROM_IDX(&attachments[i], attachments[i].sznameindex);
		std::string bonename = STRING_FROM_IDX(&bones[attachments[i].localbone], bones[attachments[i].localbone].sznameindex);
		modelOut.AddAttachment(name, bonename, attachments[i].localmatrix);
	}

	auto* bodyparts = PTR_FROM_IDX(r5::v10::mstudiobodyparts_t, rmdlHdr, rmdlHdr->bodypartindex);
	for (int i = 0; i < rmdlHdr->numbodyparts; i++) {
		qc::QCModel::bodypart bodypart{};
		std::string name = STRING_FROM_IDX(&bodyparts[i], bodyparts[i].sznameindex);
		auto* modeldesc = PTR_FROM_IDX(r5::v16::mstudiomodel_t, &bodyparts[i], bodyparts[i].modelindex);
		int modelnameiter = 0;
		for(int j = 0; j < bodyparts[i].nummodels; j++) {
			std::string modelname = "";
			if (modeldesc[j].nummeshes) {
				modelname = name + "_" + std::to_string(modelnameiter);
				modelnameiter++;
			}
			bodypart.models.push_back(modelname);
		}
		bodypart.name = name;
		modelOut.AddBodyPart(bodypart);
	}


	delete[] buffer;
}