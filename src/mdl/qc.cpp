#include <pch.h>
#include <define.h>
#include <mdl/studio.h>
#include <mdl/qc.h>

std::string getName(std::string anim_name) {
	std::string name;
	std::replace(anim_name.begin(), anim_name.end(), '\\', '/');
	size_t slash_pos = anim_name.find_last_of('/');
	name = (slash_pos != std::string::npos) ? anim_name.substr(slash_pos + 1) : anim_name;
	return name;
}

/* RSEQ */
void ParseNode(qc::Sequence& seq, short entrynode, short exitnode) {
	if ((entrynode != exitnode) && (entrynode != 0) && (exitnode != 0)) {
		seq.AddTransition(entrynode, exitnode);
	}
	else if ((entrynode == exitnode) && (entrynode != 0)) {
		seq.AddSeqNode(entrynode);
	}
	else if ((entrynode != exitnode) && (entrynode == 0)) {
		seq.AddExitNode(exitnode);
	}
	else if ((entrynode != exitnode) && (exitnode == 0)) {
		seq.AddEntryNode(entrynode);
	}
}

/* RRIG */
/* V10 */
template <typename T>
void ParseBones_v10(qc::QCModel& modelOut, T* hdr) {
	modelOut.bones.clear();
	auto* bones = PTR_FROM_IDX(r5::v10::mstudiobone_t, hdr, hdr->boneindex);
	for (int i = 0; i < hdr->numbones; i++) {
		std::string name = STRING_FROM_IDX(&bones[i], bones[i].sznameindex);
		int parent_index = bones[i].parent;
		std::string parentname;
		if (parent_index >= 0) {
			parentname = STRING_FROM_IDX(&bones[bones[i].parent], bones[bones[i].parent].sznameindex);
		}
		qc::QCModel::bonedata bonedata;
		bonedata.name = name;
		bonedata.parentName = parent_index == -1 ? "" : parentname;
		bonedata.pos = bones[i].pos;
		bonedata.rot = bones[i].rot;
		modelOut.AddBone(bonedata);
	}
}

template <typename T>
void ParseAttachments_v10(qc::QCModel& modelOut, T* hdr) {
	auto* bones = PTR_FROM_IDX(r5::v10::mstudiobone_t, hdr, hdr->boneindex);
	auto* attachments = PTR_FROM_IDX(r5::v10::mstudioattachment_t, hdr, hdr->localattachmentindex);
	for (int i = 0; i < hdr->numlocalattachments; i++) {
		std::string name = STRING_FROM_IDX(&attachments[i], attachments[i].sznameindex);
		std::string bonename = STRING_FROM_IDX(&bones[attachments[i].localbone], bones[attachments[i].localbone].sznameindex);
		modelOut.AddAttachment(name, bonename, attachments[i].localmatrix);
	}
}

template <typename T>
void ParseBodyParts_v10(qc::QCModel& modelOut, T* hdr) {
	auto* bodyparts = PTR_FROM_IDX(r5::v10::mstudiobodyparts_t, hdr, hdr->bodypartindex);
	for (int i = 0; i < hdr->numbodyparts; i++) {
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
}

template <typename T>
void ParsePoseParams_v10(qc::QCModel& modelOut, T* hdr) {
	auto* poseparamDesc = PTR_FROM_IDX(r5::v10::mstudioposeparamdesc_t, hdr, hdr->localposeparamindex);
	for (int i = 0; i < hdr->numlocalposeparameters; i++) {
		std::string poseparam_name = STRING_FROM_IDX(&poseparamDesc[i], poseparamDesc[i].sznameindex);
		modelOut.poseparam.push_back({ poseparam_name,poseparamDesc[i].flags, poseparamDesc[i].start, poseparamDesc[i].end, poseparamDesc[i].loop }); // flags?
	}

}

template <typename T>
void ParseNodeNames_v10(qc::QCModel& modelOut, T* hdr) {
	auto* nodename_index = PTR_FROM_IDX(int, hdr, hdr->localnodenameindex);
	for (int i = 0; i < hdr->numlocalnodes; i++) {
		std::string node_name = STRING_FROM_IDX(hdr, nodename_index[i]);
		modelOut.AddNode(i + 1, node_name);
	}
}

template <typename T>
void ParseIkChains_v10(qc::QCModel& modelOut, T* hdr) {
	r5::v10::mstudioikchain_t* ikchainDesc = PTR_FROM_IDX(r5::v10::mstudioikchain_t, hdr, hdr->ikchainindex);
	for (int i = 0; i < hdr->numikchains; i++) {
		int* iklink_index = PTR_FROM_IDX(int, &ikchainDesc[i], ikchainDesc[i].linkindex);
		std::string chain_name = STRING_FROM_IDX(ikchainDesc, ikchainDesc[i].sznameindex);
		std::string bone_name = modelOut.bones[iklink_index[2]].name;
		modelOut.ikchain.push_back({ chain_name, bone_name, ikchainDesc[i].unk });
	}
}

/* V12.1 */
template <typename T>
void ParseBones_v121(qc::QCModel& modelOut, T* hdr) {
	modelOut.bones.clear();
	auto* bones = PTR_FROM_IDX(r5::v121::mstudiobone_t, hdr, hdr->boneindex);
	for (int i = 0; i < hdr->numbones; i++) {
		std::string name = STRING_FROM_IDX(&bones[i], bones[i].sznameindex);
		int parent_index = bones[i].parent;
		std::string parentname;
		if (parent_index >= 0) {
			parentname = STRING_FROM_IDX(&bones[parent_index], bones[parent_index].sznameindex);
		}
		qc::QCModel::bonedata bonedata;
		bonedata.name = name;
		bonedata.parentName = parent_index == -1 ? "" : parentname;
		bonedata.pos = bones[i].pos;
		bonedata.rot = bones[i].rot;
		modelOut.AddBone(bonedata);
	}
}

template <typename T>
void ParseAttachments_v121(qc::QCModel& modelOut, T* hdr) {
	auto* bones = PTR_FROM_IDX(r5::v121::mstudiobone_t, hdr, hdr->boneindex);
	auto* attachments = PTR_FROM_IDX(r5::v10::mstudioattachment_t, hdr, hdr->localattachmentindex);
	for (int i = 0; i < hdr->numlocalattachments; i++) {
		std::string name = STRING_FROM_IDX(&attachments[i], attachments[i].sznameindex);
		std::string bonename = STRING_FROM_IDX(&bones[attachments[i].localbone], bones[attachments[i].localbone].sznameindex);
		modelOut.AddAttachment(name, bonename, attachments[i].localmatrix);
	}
}

template <typename T>
void ParseBodyParts_v121(qc::QCModel& modelOut, T* hdr) {
	auto* bodyparts = PTR_FROM_IDX(r5::v10::mstudiobodyparts_t, hdr, hdr->bodypartindex);
	for (int i = 0; i < hdr->numbodyparts; i++) {
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
}

/* V16 */
void ParseBones_v16(qc::QCModel& modelOut, r5::v16::studiohdr_t* rrigHdr) {
	modelOut.bones.clear();
	auto* raw_bone = PTR_FROM_IDX(r5::v16::mstudiobone_t, rrigHdr, rrigHdr->boneindex);
	auto* pBonedata = PTR_FROM_IDX(r5::v16::mstudiobonedata_t, rrigHdr, rrigHdr->bonedataindex);

	for (int i = 0; i < rrigHdr->numbones; i++) {
		short parent_index = pBonedata[i].parent;
		std::string name = STRING_FROM_IDX(&raw_bone[i], raw_bone[i].sznameindex);
		std::string parentname = STRING_FROM_IDX(&raw_bone[parent_index], raw_bone[parent_index].sznameindex);

		qc::QCModel::bonedata bonedata;
		bonedata.name = name;
		bonedata.parentName = parent_index == -1 ? "" : parentname;
		bonedata.pos = pBonedata[i].pos;
		bonedata.rot = pBonedata[i].rot;
		modelOut.AddBone(bonedata);
	}
}

void ParseAttachments_v16(qc::QCModel& modelOut, r5::v16::studiohdr_t* hdr) {
	auto* bones = PTR_FROM_IDX(r5::v16::mstudiobone_t, hdr, hdr->boneindex);
	auto* attachments = PTR_FROM_IDX(r5::v16::mstudioattachment_t, hdr, hdr->localattachmentindex);
	for (int i = 0; i < hdr->numlocalattachments; i++) {
		std::string name = STRING_FROM_IDX(&attachments[i], attachments[i].sznameindex);
		std::string bonename = STRING_FROM_IDX(&bones[attachments[i].localbone], bones[attachments[i].localbone].sznameindex);
		modelOut.AddAttachment(name, bonename, attachments[i].localmatrix);
	}
}

void ParseBodyParts_v16(qc::QCModel& modelOut, r5::v16::studiohdr_t* hdr) {
	auto* bodyparts = PTR_FROM_IDX(r5::v10::mstudiobodyparts_t, hdr, hdr->bodypartindex);
	for (int i = 0; i < hdr->numbodyparts; i++) {
		qc::QCModel::bodypart bodypart{};
		std::string name = STRING_FROM_IDX(&bodyparts[i], bodyparts[i].sznameindex);
		auto* modeldesc = PTR_FROM_IDX(r5::v16::mstudiomodel_t, &bodyparts[i], bodyparts[i].modelindex);
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
}

void ParsePoseParams_v16(qc::QCModel& modelOut, r5::v16::studiohdr_t* hdr) {
	r5::v16::mstudioposeparamdesc_t* poseparamDesc = PTR_FROM_IDX(r5::v16::mstudioposeparamdesc_t, hdr, hdr->localposeparamindex);
	for (int i = 0; i < hdr->numlocalposeparameters; i++) {
		std::string poseparam_name = STRING_FROM_IDX(&poseparamDesc[i], poseparamDesc[i].sznameindex);
		modelOut.poseparam.push_back({ poseparam_name,poseparamDesc[i].flags, poseparamDesc[i].start, poseparamDesc[i].end, poseparamDesc[i].loop });
	}
}

void ParseNodeNames_v16(qc::QCModel& modelOut, r5::v16::studiohdr_t* hdr) {
	auto* nodename_index = PTR_FROM_IDX(short, hdr, hdr->localnodenameindex);
	for (int i = 0; i < hdr->numlocalnodes; i++) {
		std::string node_name = STRING_FROM_IDX(nodename_index, nodename_index[i]);
		modelOut.AddNode(i + 1, node_name);
	}
}

void ParseIkChains_v16(qc::QCModel& modelOut, r5::v16::studiohdr_t* hdr){
	r5::v16::mstudioikchain_t * ikchainDesc = PTR_FROM_IDX(r5::v16::mstudioikchain_t, hdr, hdr->ikchainindex);
	for (int i = 0; i < hdr->numikchains; i++) {
		int* iklink_index = PTR_FROM_IDX(int, &ikchainDesc[i], ikchainDesc[i].linkindex);
		std::string chain_name = STRING_FROM_IDX(ikchainDesc, ikchainDesc[i].sznameindex);
		std::string bone_name = modelOut.bones[iklink_index[2 * 4]].name;
		modelOut.ikchain.push_back({ chain_name, bone_name, ikchainDesc[i].unk });
	} 
}

/* V17 */
template <typename T>
void ParseBones_v17(qc::QCModel& modelOut, T* hdr) {
	modelOut.bones.clear();
	auto* raw_bone = PTR_FROM_IDX(r5::v16::mstudiobone_t, hdr, OFFSET(hdr->boneindex));
	auto* pBonedata = PTR_FROM_IDX(r5::v16::mstudiobonedata_t, hdr, OFFSET(hdr->bonedataindex));

	for (int i = 0; i < hdr->numbones; i++) {
		short parent_index = pBonedata[i].parent;
		std::string name = STRING_FROM_IDX(&raw_bone[i], raw_bone[i].sznameindex);
		std::string parentname = STRING_FROM_IDX(&raw_bone[parent_index], raw_bone[parent_index].sznameindex);

		qc::QCModel::bonedata bonedata;
		bonedata.name = name;
		bonedata.parentName = parent_index == -1 ? "" : parentname;
		bonedata.pos = pBonedata[i].pos;
		bonedata.rot = pBonedata[i].rot;
		modelOut.AddBone(bonedata);
	}
}

template <typename T>
void ParseAttachments_v17(qc::QCModel& modelOut, T* hdr) {
	auto* bones = PTR_FROM_IDX(r5::v16::mstudiobone_t, hdr, OFFSET(hdr->boneindex));
	auto* attachments = PTR_FROM_IDX(r5::v16::mstudioattachment_t, hdr, OFFSET(hdr->localattachmentindex));
	for (int i = 0; i < hdr->numlocalattachments; i++) {
		std::string name = STRING_FROM_IDX(&attachments[i], attachments[i].sznameindex);
		std::string bonename = STRING_FROM_IDX(&bones[attachments[i].localbone], bones[attachments[i].localbone].sznameindex);
		modelOut.AddAttachment(name, bonename, attachments[i].localmatrix);
	}
}

template <typename T>
void ParseBodyParts_v17(qc::QCModel& modelOut, T* hdr) {
	auto* bodyparts = PTR_FROM_IDX(r5::v10::mstudiobodyparts_t, hdr, OFFSET(hdr->bodypartindex));
	for (int i = 0; i < hdr->numbodyparts; i++) {
		qc::QCModel::bodypart bodypart{};
		std::string name = STRING_FROM_IDX(&bodyparts[i], bodyparts[i].sznameindex);
		auto* modeldesc = PTR_FROM_IDX(r5::v16::mstudiomodel_t, &bodyparts[i], bodyparts[i].modelindex);
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
}

template <typename T>
void ParsePoseParams_v17(qc::QCModel& modelOut, T* hdr) {
	r5::v16::mstudioposeparamdesc_t* poseparamDesc = PTR_FROM_IDX(r5::v16::mstudioposeparamdesc_t, hdr, OFFSET(hdr->localposeparamindex));
	for (int i = 0; i < hdr->numlocalposeparameters; i++) {
		std::string poseparam_name = STRING_FROM_IDX(&poseparamDesc[i], poseparamDesc[i].sznameindex);
		modelOut.poseparam.push_back({ poseparam_name,poseparamDesc[i].flags, poseparamDesc[i].start, poseparamDesc[i].end, poseparamDesc[i].loop });
	}
}

template <typename T>
void ParseNodeNames_v17(qc::QCModel& modelOut, T* hdr) {
	auto* nodename_index = PTR_FROM_IDX(short, hdr, OFFSET(hdr->localnodenameindex));
	for (int i = 0; i < hdr->numlocalnodes; i++) {
		std::string node_name = STRING_FROM_IDX(nodename_index, nodename_index[i]);
		modelOut.AddNode(i + 1, node_name);
	}
}

template <typename T>
void ParseIkChains_v17(qc::QCModel& modelOut, T* hdr) {
	r5::v16::mstudioikchain_t* ikchainDesc = PTR_FROM_IDX(r5::v16::mstudioikchain_t, hdr, OFFSET(hdr->ikchainindex));
	for (int i = 0; i < hdr->numikchains; i++) {
		int* iklink_index = PTR_FROM_IDX(int, &ikchainDesc[i], ikchainDesc[i].linkindex);
		std::string chain_name = STRING_FROM_IDX(ikchainDesc, ikchainDesc[i].sznameindex);
		std::string bone_name = modelOut.bones[iklink_index[2 * 4]].name;
		modelOut.ikchain.push_back({ chain_name, bone_name, ikchainDesc[i].unk });
	}
}

/* V19 */
template <typename T>
void ParseBones_v19(qc::QCModel& modelOut, T* hdr) {
	modelOut.bones.clear();
	auto* raw_bone = PTR_FROM_IDX(r5::v16::mstudiobone_t, hdr, OFFSET(hdr->boneindex));
	auto* linearboneindex = PTR_FROM_IDX(r5::v16::linearboneindex_t, hdr, OFFSET(hdr->linearboneindex));
	auto* parentbone = PTR_FROM_IDX(short, linearboneindex, OFFSET(linearboneindex->parentindex));
	auto* studioPos = PTR_FROM_IDX(Vector3, linearboneindex, OFFSET(linearboneindex->posindex));
	auto* studioRot = PTR_FROM_IDX(RadianEuler, linearboneindex, OFFSET(linearboneindex->rotindex));
	for (int i = 0; i < hdr->numbones; i++) {
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
}

/* PARSER */
void ParseRRIG_v10(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rrig_stream(path, std::ios::binary);

	std::vector<char> buffer(inputFileSize, 0);
	rrig_stream.read(buffer.data(), inputFileSize);
	rrig_stream.close();

	auto* rrigHdr = reinterpret_cast<r5::v10::studiohdr_t*>(buffer.data());
	if (modelOut.modelname.empty())
		modelOut.modelname = STRING_FROM_IDX(rrigHdr, rrigHdr->sznameindex);
	modelOut.illumposition = rrigHdr->illumposition;
	modelOut.cboxmin = rrigHdr->hull_min;
	modelOut.cboxmax = rrigHdr->hull_max;
	modelOut.bboxmin = rrigHdr->view_bbmin;
	modelOut.bboxmax = rrigHdr->view_bbmax;

	ParseBones_v10(modelOut, rrigHdr);
	ParseAttachments_v10(modelOut, rrigHdr);
	ParseBodyParts_v10(modelOut, rrigHdr);
	ParsePoseParams_v10(modelOut, rrigHdr);
	ParseNodeNames_v10(modelOut, rrigHdr);
	ParseIkChains_v10(modelOut, rrigHdr);
}

void ParseRRIG_v121(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rrig_stream(path, std::ios::binary);

	std::vector<char> buffer(inputFileSize, 0);
	rrig_stream.read(buffer.data(), inputFileSize);
	rrig_stream.close();

	auto* rrigHdr = reinterpret_cast<r5::v13::studiohdr_t*>(buffer.data());
	if (modelOut.modelname.empty())
		modelOut.modelname = STRING_FROM_IDX(rrigHdr, rrigHdr->sznameindex);
	modelOut.illumposition = rrigHdr->illumposition;
	modelOut.cboxmin = rrigHdr->hull_min;
	modelOut.cboxmax = rrigHdr->hull_max;
	modelOut.bboxmin = rrigHdr->view_bbmin;
	modelOut.bboxmax = rrigHdr->view_bbmax;

	ParseBones_v121(modelOut, rrigHdr);
	ParseAttachments_v121(modelOut, rrigHdr);
	ParseBodyParts_v121(modelOut, rrigHdr);
	ParsePoseParams_v10(modelOut, rrigHdr);
	ParseNodeNames_v10(modelOut, rrigHdr);
	ParseIkChains_v10(modelOut, rrigHdr);
}

void ParseRRIG_v13(std::string path, qc::QCModel& modelOut) {
	ParseRRIG_v121(path, modelOut);
}

void ParseRRIG_v14(std::string path, qc::QCModel& modelOut) {
	ParseRRIG_v121(path, modelOut);
}

void ParseRRIG_v16(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rrig_stream(path, std::ios::binary);

	std::vector<char> buffer(inputFileSize, 0);
	rrig_stream.read(buffer.data(), inputFileSize);
	rrig_stream.close();

	r5::v16::studiohdr_t* rrigHdr = reinterpret_cast<r5::v16::studiohdr_t*>(buffer.data());
	if (modelOut.modelname.empty())
		modelOut.modelname = STRING_FROM_IDX(rrigHdr, rrigHdr->sznameindex);
	modelOut.illumposition = rrigHdr->illumposition;
	modelOut.cboxmin = rrigHdr->hull_min;
	modelOut.cboxmax = rrigHdr->hull_max;
	modelOut.bboxmin = rrigHdr->view_bbmin;
	modelOut.bboxmax = rrigHdr->view_bbmax;

	ParseBones_v16(modelOut, rrigHdr);
	ParseAttachments_v16(modelOut, rrigHdr);
	ParseBodyParts_v16(modelOut, rrigHdr);
	ParsePoseParams_v16(modelOut, rrigHdr);
	ParseNodeNames_v16(modelOut, rrigHdr);
	ParseIkChains_v16(modelOut, rrigHdr);
}

void ParseRRIG_v17(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rrig_stream(path, std::ios::binary);

	std::vector<char> buffer(inputFileSize, 0);
	rrig_stream.read(buffer.data(), inputFileSize);
	rrig_stream.close();

	auto* rrigHdr = reinterpret_cast<r5::v17::studiohdr_t*>(buffer.data());
	if (modelOut.modelname.empty())
		modelOut.modelname = STRING_FROM_IDX(rrigHdr, rrigHdr->sznameindex);
	modelOut.illumposition = rrigHdr->illumposition;
	modelOut.cboxmin = rrigHdr->hull_min;
	modelOut.cboxmax = rrigHdr->hull_max;
	modelOut.bboxmin = rrigHdr->view_bbmin;
	modelOut.bboxmax = rrigHdr->view_bbmax;

	ParseBones_v17(modelOut, rrigHdr);
	ParseAttachments_v17(modelOut, rrigHdr);
	ParseBodyParts_v17(modelOut, rrigHdr);
	ParsePoseParams_v17(modelOut, rrigHdr);
	ParseNodeNames_v17(modelOut, rrigHdr);
	ParseIkChains_v17(modelOut, rrigHdr);
}

void ParseRRIG_v18(std::string path, qc::QCModel& modelOut) {
	ParseRRIG_v17(path, modelOut);
}

void ParseRRIG_v19(std::string path, qc::QCModel& modelOut) {
	modelOut.hasRrig = true;
	size_t inputFileSize = std::filesystem::file_size(path);
	std::ifstream rrig_stream(path, std::ios::binary);

	std::vector<char> buffer(inputFileSize, 0);
	rrig_stream.read(buffer.data(), inputFileSize);
	rrig_stream.close();

	auto* rrigHdr = reinterpret_cast<r5::v19::studiohdr_t*>(buffer.data());
	if(modelOut.modelname.empty()) 
		modelOut.modelname = STRING_FROM_IDX(rrigHdr, rrigHdr->sznameindex);
	modelOut.illumposition = rrigHdr->illumposition;
	modelOut.cboxmin = rrigHdr->hull_min;
	modelOut.cboxmax = rrigHdr->hull_max;
	modelOut.bboxmin = rrigHdr->view_bbmin;
	modelOut.bboxmax = rrigHdr->view_bbmax;

	ParseBones_v19(modelOut, rrigHdr);
	ParseAttachments_v17(modelOut, rrigHdr);
	ParseBodyParts_v17(modelOut, rrigHdr);
	ParsePoseParams_v17(modelOut, rrigHdr);
	ParseNodeNames_v17(modelOut, rrigHdr);
	ParseIkChains_v17(modelOut, rrigHdr);
}

void ParseRRIG_v191(std::string path, qc::QCModel& modelOut) {
	ParseRRIG_v19(path, modelOut);
}

void ParseRSEQ_v7(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut) {
	for (const auto& file : paths) {
		size_t inputFileSize = std::filesystem::file_size(file);
		std::ifstream rseq_stream(file, std::ios::binary);
		std::vector<char> buffer(inputFileSize);
		rseq_stream.read(buffer.data(), inputFileSize);

		// bad size
		if (inputFileSize <= sizeof(r5anim::v12::mstudioseqdesc_t)) {
			printf("[!] Skipping %s (%zu byte)\n", getName(file).c_str(), inputFileSize);
			continue;
		}
		int num_frames = 0;
		float fps = 0;

		auto* rseqDesc = reinterpret_cast<r5anim::v7::mstudioseqdesc_t*>(buffer.data());

		std::string seq_name_raw = STRING_FROM_IDX(rseqDesc, rseqDesc->szlabelindex);
		std::string seq_name = seq_name_raw.substr(0, seq_name_raw.rfind('.'));
		std::string seq_act = STRING_FROM_IDX(rseqDesc, rseqDesc->szactivitynameindex);

		qc::Sequence seq(seq_name, "");

		auto* autolayers = PTR_FROM_IDX(r5anim::v7::mstudioautolayer_t, rseqDesc, rseqDesc->autolayerindex); //didn't test
		for (int i = 0; i < rseqDesc->numautolayers; i++) {
			__int64 guid = autolayers[i].guidSequence;
			seq.autolayerGuid.push_back(guid);
		}

		int* blend_idx = PTR_FROM_IDX(int, rseqDesc, rseqDesc->animindexindex);
		uint32_t blendwidth = std::max(rseqDesc->groupsize[0], rseqDesc->groupsize[1]);

		seq.blendWidth = blendwidth;

		for (int i = 0; i < rseqDesc->numblends; i++) {
			auto* animDesc = PTR_FROM_IDX(r5anim::v7::mstudioanimdesc_t, rseqDesc, blend_idx[i]);

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

			auto* ikrule = PTR_FROM_IDX(r5anim::v7::mstudioikrule_t, animDesc, animDesc->ikruleindex);
			for (int j = 0; j < animDesc->numikrules; j++) {
				rawAnim.ikrules.push_back({ ikrule[j].index });
			}

			if (animDesc->movementindex) {
				auto* movement = PTR_FROM_IDX(r5anim::v7::mstudiomovement_t, animDesc, animDesc->movementindex);
				for (int j = 0; j < animDesc->nummovements; j++) {
					rawAnim.movement.push_back({ movement->v_start, movement->v_end });
				}
			}

			if (animDesc->framemovementindex) {
				auto* movement = PTR_FROM_IDX(r5anim::v7::mstudioframemovement_t, animDesc, animDesc->framemovementindex);
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

		auto* activityMods = PTR_FROM_IDX(r5anim::v7::mstudioactivitymodifier_t, rseqDesc, rseqDesc->activitymodifierindex);
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
		auto* events = PTR_FROM_IDX(r5anim::v7::mstudioevent_t, rseqDesc, rseqDesc->eventindex);
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
		ParseNode(seq, entrynode, exitnode);
		modelOut.AddSequence(seq);
	}
}

void ParseRSEQ_v10(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut) {
	for (const auto& file : paths) {
		int num_frames = 0;
		float fps = 0;

		size_t inputFileSize = std::filesystem::file_size(file);
		std::ifstream rseq_stream(file, std::ios::binary);
		std::vector<char> buffer(inputFileSize);
		rseq_stream.read(buffer.data(), inputFileSize);

		// bad size
		if (inputFileSize <= sizeof(r5anim::v10::mstudioseqdesc_t)) {
			printf("[!] Skipping %s (%zu byte)\n", getName(file).c_str(), inputFileSize);
			continue;
		}

		auto* rseqDesc = reinterpret_cast<r5anim::v10::mstudioseqdesc_t*>(buffer.data());
		std::string seq_name_raw = STRING_FROM_IDX(rseqDesc, rseqDesc->szlabelindex);
		std::string seq_name = seq_name_raw.substr(0, seq_name_raw.rfind('.'));
		std::string seq_act = STRING_FROM_IDX(rseqDesc, rseqDesc->szactivitynameindex);
		qc::Sequence seq(seq_name, "");

		auto* autolayers = PTR_FROM_IDX(r5anim::v7::mstudioautolayer_t, rseqDesc, rseqDesc->autolayerindex);
		for (int i = 0; i < rseqDesc->numautolayers; i++) {
			__int64 guid = autolayers[i].guidSequence;
			seq.autolayerGuid.push_back(guid);
		}

		int* blend_idx = PTR_FROM_IDX(int, rseqDesc, rseqDesc->animindexindex);
		uint32_t blendwidth = std::max(rseqDesc->groupsize[0], rseqDesc->groupsize[1]);
		seq.blendWidth = blendwidth;

		for (int i = 0; i < rseqDesc->numblends; i++) {
			auto* animDesc = PTR_FROM_IDX(r5anim::v10::mstudioanimdesc_t, rseqDesc, blend_idx[i]);

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

			auto* ikrule = PTR_FROM_IDX(r5anim::v10::mstudioikrule_t, animDesc, animDesc->ikruleindex);
			for (int j = 0; j < animDesc->numikrules; j++) {
				if (ikrule[j].type != 4) printf("ikrule %d\n", ikrule[j].type);
				rawAnim.ikrules.push_back({ ikrule[j].index });
			}

			if (animDesc->framemovementindex) {
				auto* movement = PTR_FROM_IDX(r5anim::v7::mstudioframemovement_t, animDesc, animDesc->framemovementindex);
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

		auto* activityMods = PTR_FROM_IDX(r5anim::v10::mstudioactivitymodifier_t, rseqDesc, rseqDesc->activitymodifierindex);
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
		auto* events = PTR_FROM_IDX(r5anim::v10::mstudioevent_2_t, rseqDesc, rseqDesc->eventindex);
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
		ParseNode(seq, entrynode, exitnode);
		modelOut.AddSequence(seq);
	}
}

void ParseRSEQ_v11(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut) {
	for (const auto& file : paths) {
		int num_frames = 0;
		float fps = 0;

		size_t inputFileSize = std::filesystem::file_size(file);
		std::ifstream rseq_stream(file, std::ios::binary);
		std::vector<char> buffer(inputFileSize);
		rseq_stream.read(buffer.data(), inputFileSize);

		// bad size
		if (inputFileSize <= sizeof(r5anim::v11::mstudioseqdesc_t)) {
			printf("[!] Skipping %s (%zu byte)\n", getName(file).c_str(), inputFileSize);
			continue;
		}

		auto* rseqDesc = reinterpret_cast<r5anim::v11::mstudioseqdesc_t*>(buffer.data());
		std::string seq_name_raw = STRING_FROM_IDX(rseqDesc, OFFSET(rseqDesc->szlabelindex));
		std::string seq_name = seq_name_raw.substr(0, seq_name_raw.rfind('.'));
		std::string seq_act = STRING_FROM_IDX(rseqDesc, OFFSET(rseqDesc->szactivitynameindex));

		qc::Sequence seq(seq_name, "");
		int numbones = (OFFSET(rseqDesc->activitymodifierindex) - OFFSET(rseqDesc->weightlistindex)) / 4;

		if (OFFSET(rseqDesc->weightlistindex) >= OFFSET(rseqDesc->autolayerindex)) {
			if (modelOut.bones.size() > numbones) {
				printf("[!] Warning: %s bone count is mismatched.\n", seq_name_raw.c_str());
				goto SKIP_WEIGHTLIST;
			}
			std::vector<float> weightlist{};
			int numHasValue = 0;
			float* weight = PTR_FROM_IDX(float, rseqDesc, OFFSET(rseqDesc->weightlistindex));
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
		SKIP_WEIGHTLIST:

		auto* autolayers = PTR_FROM_IDX(r5anim::v7::mstudioautolayer_t, rseqDesc, OFFSET(rseqDesc->autolayerindex));
		for (int i = 0; i < rseqDesc->numautolayers; i++) {
			auto guid = autolayers[i].guidSequence;
			seq.autolayerGuid.push_back(guid);
		}

		auto* blend_idx = PTR_FROM_IDX(uint16_t, rseqDesc, OFFSET(rseqDesc->animindexindex));
		uint32_t blendwidth = std::max(rseqDesc->groupsize[0], rseqDesc->groupsize[1]);
		seq.blendWidth = blendwidth;
		//
		for (int i = 0; i < rseqDesc->numblends; i++) {
			auto* animDesc = PTR_FROM_IDX(r5anim::v11::mstudioanimdesc_t, rseqDesc, blend_idx[i]);

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

			auto* ikrule = PTR_FROM_IDX(r5anim::v11::mstudioikrule_t, animDesc, animDesc->ikruleindex);
			for (int j = 0; j < animDesc->numikrules; j++) {
				rawAnim.ikrules.push_back({ ikrule[j].type });
			}

			if (animDesc->framemovementindex) {
				auto* movement = PTR_FROM_IDX(r5anim::v7::mstudioframemovement_t, animDesc, animDesc->framemovementindex);
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

		auto* activityMods = PTR_FROM_IDX(r5anim::v11::mstudioactivitymodifier_t, rseqDesc, OFFSET(rseqDesc->activitymodifierindex));
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
		auto* events = PTR_FROM_IDX(r5anim::v11::mstudioevent_t, rseqDesc, OFFSET(rseqDesc->eventindex));
		for (int i = 0; i < rseqDesc->numevents; i++) {
			std::string eventName = STRING_FROM_IDX(&events[i], events[i].szeventindex);
			std::string eventOption = STRING_FROM_IDX(&events[i], events[i].optionsindex);
			int frame = static_cast<int>(events[i].cycle * num_frames);

			std::stringstream ss;
			ss << eventName << " " << frame << " \"" << eventOption << "\"";
			temp::event_t event(frame, eventName, eventOption);
			seq.AddEvent(event);
		}

		// Nodes / Transitions
		short entrynode = rseqDesc->localentrynode;
		short exitnode = rseqDesc->localexitnode;
		ParseNode(seq, entrynode, exitnode);
		modelOut.AddSequence(seq);
	}
}

void ParseRSEQ_v12(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut) {
	for (const auto& file : paths) {
		int num_frames = 0;
		float fps = 0;

		size_t inputFileSize = std::filesystem::file_size(file);
		std::ifstream rseq_stream(file, std::ios::binary);
		std::vector<char> buffer(inputFileSize);
		rseq_stream.read(buffer.data(), inputFileSize);

		// bad size
		if (inputFileSize <= sizeof(r5anim::v12::mstudioseqdesc_t)) {
			printf("[!] Skipping %s (%zu byte)\n", getName(file).c_str(), inputFileSize);
			continue;
		}

		auto* rseqDesc = reinterpret_cast<r5anim::v12::mstudioseqdesc_t*>(buffer.data());
		std::string seq_name_raw = STRING_FROM_IDX(rseqDesc, OFFSET(rseqDesc->szlabelindex));
		std::string seq_name = seq_name_raw.substr(0, seq_name_raw.rfind('.'));
		std::string seq_act = STRING_FROM_IDX(rseqDesc, OFFSET(rseqDesc->szactivitynameindex));

		qc::Sequence seq(seq_name, "");
		int numbones = (OFFSET(rseqDesc->activitymodifierindex) - OFFSET(rseqDesc->weightlistindex)) / 4;

		if (OFFSET(rseqDesc->weightlistindex) >= OFFSET(rseqDesc->autolayerindex)) {
			std::vector<float> weightlist{};
			int numHasValue = 0;
			float* weight = PTR_FROM_IDX(float, rseqDesc, OFFSET(rseqDesc->weightlistindex));
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

		auto* autolayers = PTR_FROM_IDX(r5anim::v7::mstudioautolayer_t, rseqDesc, OFFSET(rseqDesc->autolayerindex));
		for (int i = 0; i < rseqDesc->numautolayers; i++) {
			auto guid = autolayers[i].guidSequence;
			seq.autolayerGuid.push_back(guid);
		}

		auto* blend_idx = PTR_FROM_IDX(uint16_t, rseqDesc, OFFSET(rseqDesc->animindexindex));
		uint32_t blendwidth = std::max(rseqDesc->groupsize[0], rseqDesc->groupsize[1]);
		seq.blendWidth = blendwidth;

		for (int i = 0; i < rseqDesc->numblends; i++) {
			auto* animDesc = PTR_FROM_IDX(r5anim::v12::mstudioanimdesc_t, rseqDesc, OFFSET(blend_idx[i]));
			num_frames = std::max(num_frames, animDesc->numframes - 1);
			fps = std::max(fps, animDesc->fps);
			std::string anim_name = STRING_FROM_IDX(animDesc, OFFSET(animDesc->sznameindex));
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
				auto* movement = PTR_FROM_IDX(r5anim::v7::mstudioframemovement_t, animDesc, OFFSET(animDesc->framemovementindex));
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

		auto* activityMods = PTR_FROM_IDX(r5anim::v12::mstudioactivitymodifier_t, rseqDesc, OFFSET(rseqDesc->activitymodifierindex));
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
		auto* events = PTR_FROM_IDX(r5anim::v12::mstudioevent_t, rseqDesc, OFFSET(rseqDesc->eventindex));
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
		ParseNode(seq, entrynode, exitnode);
		modelOut.AddSequence(seq);
	}
}

void ParseRSEQ_v121(const std::string in_dir, const std::vector<std::string>& paths, qc::QCModel& modelOut) {
	for (const auto& file : paths) {
		int num_frames = 0;
		float fps = 0;

		size_t inputFileSize = std::filesystem::file_size(file);
		std::ifstream rseq_stream(file, std::ios::binary);
		std::vector<char> buffer(inputFileSize);
		rseq_stream.read(buffer.data(), inputFileSize);

		// bad size
		if (inputFileSize <= sizeof(r5anim::v12::mstudioseqdesc_t)) {
			printf("[!] Skipping %s (%zu byte)\n", getName(file).c_str(), inputFileSize);
			continue;
		}

		auto* rseqDesc = reinterpret_cast<r5anim::v12::mstudioseqdesc_t*>(buffer.data());
		std::string seq_name_raw = STRING_FROM_IDX(rseqDesc, OFFSET(rseqDesc->szlabelindex));
		std::string seq_name = seq_name_raw.substr(0, seq_name_raw.rfind('.'));
		std::string seq_act = STRING_FROM_IDX(rseqDesc, OFFSET(rseqDesc->szactivitynameindex));

		qc::Sequence seq(seq_name, "");
		int numbones = (OFFSET(rseqDesc->activitymodifierindex) - OFFSET(rseqDesc->weightlistindex)) / 4;

		if (OFFSET(rseqDesc->weightlistindex) >= OFFSET(rseqDesc->autolayerindex)) {
			std::vector<float> weightlist{};
			int numHasValue = 0;
			float* weight = PTR_FROM_IDX(float, rseqDesc, OFFSET(rseqDesc->weightlistindex));
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

		auto* autolayers = PTR_FROM_IDX(r5anim::v7::mstudioautolayer_t, rseqDesc, OFFSET(rseqDesc->autolayerindex));
		for (int i = 0; i < rseqDesc->numautolayers; i++) {
			auto guid = autolayers[i].guidSequence;
			seq.autolayerGuid.push_back(guid);
		}

		auto* blend_idx = PTR_FROM_IDX(uint16_t, rseqDesc, OFFSET(rseqDesc->animindexindex));
		uint32_t blendwidth = std::max(rseqDesc->groupsize[0], rseqDesc->groupsize[1]);
		seq.blendWidth = blendwidth;

		for (int i = 0; i < rseqDesc->numblends; i++) {
			auto* animDesc = PTR_FROM_IDX(r5anim::v121::mstudioanimdesc_t, rseqDesc, OFFSET(blend_idx[i]));
			num_frames = std::max(num_frames, animDesc->numframes - 1);
			fps = std::max(fps, animDesc->fps);
			std::string anim_name = STRING_FROM_IDX(animDesc, OFFSET(animDesc->sznameindex));
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

			for (int j = 0; j < animDesc->numikrules; j++) {
				rawAnim.ikrules.push_back({ 0 });
			}

			if (animDesc->framemovementindex) {
				auto* movement = PTR_FROM_IDX(r5anim::v7::mstudioframemovement_t, animDesc, OFFSET(animDesc->framemovementindex));
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

		auto* activityMods = PTR_FROM_IDX(r5anim::v12::mstudioactivitymodifier_t, rseqDesc, OFFSET(rseqDesc->activitymodifierindex));
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
		auto* events = PTR_FROM_IDX(r5anim::v12::mstudioevent_t, rseqDesc, OFFSET(rseqDesc->eventindex));
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
		ParseNode(seq, entrynode, exitnode);
		modelOut.AddSequence(seq);
	}
}
