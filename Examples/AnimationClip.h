#pragma once

#include <directxtk/SimpleMath.h>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace hlab {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Vector3;
using std::map;
using std::string;
using std::vector;
using std::set;

struct AnimationClip {

    struct Key {
        Vector3 pos = Vector3(0.0f);
        Vector3 scale = Vector3(1.0f);
        Quaternion rot = Quaternion();

        Matrix GetTransform() {
            return Matrix::CreateScale(scale) *
                   Matrix::CreateFromQuaternion(rot) *
                   Matrix::CreateTranslation(pos);
        }
    };

    string name;              // Name of this animation clip
    vector<vector<Key>> keys; // m_key[boneIdx][frameIdx]
    int numChannels;          // Number of bones
    int numKeys;              // Number of frames of this animation clip
    double duration;          // Duration of animation in ticks
    double ticksPerSec;       // Frames per second
};

struct AnimationData {

    map<string, int32_t> boneNameToId; // 뼈 이름과 인덱스 정수
    vector<string> boneIdToName; // boneNameToId의 Id 순서대로 뼈 이름 저장
    vector<int32_t> boneParents; // 부모 뼈의 인덱스
    vector<Matrix> offsetMatrices;
    vector<Matrix> boneTransforms;
    vector<AnimationClip> clips;
    //새로 추가, 완성되면 clips를 제거
    map<string, AnimationClip> clipMaps;
    Matrix defaultTransform;
    Matrix rootTransform = Matrix();
    Matrix accumulatedRootTransform = Matrix();
    //TODO 이거 값 Actor나 Model로 보내는게..ㅎ
    Vector3 prevPos = Vector3(0.0f);
    set<int> lowerBodyBones;
    Matrix Get(int clipId, int boneId, int frame) {

        return defaultTransform.Invert() * offsetMatrices[boneId] *
               boneTransforms[boneId] * defaultTransform;

        // defaultTransform은 모델을 읽어들일때 GeometryGenerator::Normalize()
        // 에서 계산 defaultTransform.Invert() * offsetMatrices[boneId]를 미리
        // 계산해서 합치고 defaultTransform * rootTransform을 미리 계산해놓을
        // 수도 있습니다. 여기는 교육용 예제라서 좌표계 변환 사례로 참고하시라고
        // 남겨놨습니다.
    }
    Matrix GetAnimationTransform(int clipId, int boneId, int frame) {

        return defaultTransform.Invert() * offsetMatrices[boneId] *
            boneTransforms[boneId] * defaultTransform;

        // defaultTransform은 모델을 읽어들일때 GeometryGenerator::Normalize()
        // 에서 계산 defaultTransform.Invert() * offsetMatrices[boneId]를 미리
        // 계산해서 합치고 defaultTransform * rootTransform을 미리 계산해놓을
        // 수도 있습니다. 여기는 교육용 예제라서 좌표계 변환 사례로 참고하시라고
        // 남겨놨습니다.
    }
    Matrix GetAnimationTransform(int InBoneId, Matrix InBoneTransform)
    {
        return defaultTransform.Invert() * offsetMatrices[InBoneId] *
            InBoneTransform * defaultTransform;
    }
    void Update(int clipId, int frame, int type =0) {

        auto &clip = clips[clipId];

        for (int boneId = 0; boneId < boneTransforms.size(); boneId++) {
            // 1일때 상체
            if (type==1) {
              std::set<int>::iterator itr = lowerBodyBones.find(boneId);
              if (itr != lowerBodyBones.end()) {
                continue;
              }
            }
            else if (type==2) {
              std::set<int>::iterator itr = lowerBodyBones.find(boneId);
              if (itr == lowerBodyBones.end()) {
                continue;
              }
            }
            auto &keys = clip.keys[boneId];

            // 주의: 모든 채널(뼈)이 frame 개수가 동일하진 않음

            const int parentIdx = boneParents[boneId];
            const Matrix parentMatrix = parentIdx >= 0
                                            ? boneTransforms[parentIdx]
                                            : accumulatedRootTransform;

            // keys.size()가 0일 경우에는 Identity 변환
            auto key = keys.size() > 0
                           ? keys[frame % keys.size()]
                           : AnimationClip::Key(); // key가 reference 아님

            // Root일 경우
            if (parentIdx < 0) {
                if (frame != 0) {
                    accumulatedRootTransform =
                        Matrix::CreateTranslation(key.pos - prevPos) *
                        accumulatedRootTransform;
                } else {
                    auto temp = accumulatedRootTransform.Translation();
                    temp.y = key.pos.y; // 높이 방향만 첫 프레임으로 보정
                    accumulatedRootTransform.Translation(temp);
                }

                prevPos = key.pos;
                key.pos = Vector3(0.0f); // 대신에 이동 취소
            }

            // TODO: parentMatrix 사용
            // boneTransforms[boneId] = ...;
            boneTransforms[boneId] = key.GetTransform()*  parentMatrix;
        }
    }

     bool GetBoneTransform(string clipId, int frame, Matrix& InRootTransform, vector<Matrix>&  OutBoneTransform, int type = 0) {
         //TODO 제거
        auto& clip = clipMaps[clipId];

        for (int boneId = 0; boneId < boneTransforms.size(); boneId++) {
            // 1일때 상체
            if (type == 1) {
                std::set<int>::iterator itr = lowerBodyBones.find(boneId);
                if (itr != lowerBodyBones.end()) {
                    continue;
                }
            }
            else if (type == 2) {
                std::set<int>::iterator itr = lowerBodyBones.find(boneId);
                if (itr == lowerBodyBones.end()) {
                    continue;
                }
            }
            auto& keys = clip.keys[boneId];

            // 주의: 모든 채널(뼈)이 frame 개수가 동일하진 않음

            const int parentIdx = boneParents[boneId];
            const Matrix parentMatrix = parentIdx >= 0
                ? OutBoneTransform[parentIdx]
                : InRootTransform;

            // keys.size()가 0일 경우에는 Identity 변환
            auto key = keys.size() > 0
                ? keys[frame % keys.size()]
                : AnimationClip::Key(); // key가 reference 아님

            // Root일 경우
            if (parentIdx < 0) {
                if (frame != 0) {
                    InRootTransform =
                        Matrix::CreateTranslation(key.pos - prevPos) *
                        InRootTransform;
                }
                else {
                    auto temp = InRootTransform.Translation();
                    temp.y = key.pos.y; // 높이 방향만 첫 프레임으로 보정
                    InRootTransform.Translation(temp);
                }

                prevPos = key.pos;
                key.pos = Vector3(0.0f); // 대신에 이동 취소
            }

            // TODO: parentMatrix 사용
            // boneTransforms[boneId] = ...;
            OutBoneTransform[boneId] = key.GetTransform() * parentMatrix;
        }
        return true;
    }
};

} // namespace hlab