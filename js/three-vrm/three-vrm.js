/*!
 * @pixiv/three-vrm v0.6.2
 * VRM file loader for three.js.
 *
 * Copyright (c) 2019-2021 pixiv Inc.
 * @pixiv/three-vrm is distributed under MIT License
 * https://github.com/pixiv/three-vrm/blob/release/LICENSE
 */
(function (global, factory) {
    typeof exports === 'object' && typeof module !== 'undefined' ? factory(exports, require('three')) :
    typeof define === 'function' && define.amd ? define(['exports', 'three'], factory) :
    (global = typeof globalThis !== 'undefined' ? globalThis : global || self, factory(global.THREE_VRM = {}, global.THREE));
}(this, (function (exports, THREE) { 'use strict';

    /*! *****************************************************************************
    Copyright (c) Microsoft Corporation.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
    REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
    INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
    OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.
    ***************************************************************************** */

    function __awaiter(thisArg, _arguments, P, generator) {
        function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
        return new (P || (P = Promise))(function (resolve, reject) {
            function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
            function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
            function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
            step((generator = generator.apply(thisArg, _arguments || [])).next());
        });
    }

    // See: https://threejs.org/docs/#manual/en/introduction/How-to-dispose-of-objects
    function disposeMaterial(material) {
        Object.keys(material).forEach((propertyName) => {
            const value = material[propertyName];
            if (value === null || value === void 0 ? void 0 : value.isTexture) {
                const texture = value;
                texture.dispose();
            }
        });
        material.dispose();
    }
    function dispose(object3D) {
        const geometry = object3D.geometry;
        if (geometry) {
            geometry.dispose();
        }
        const material = object3D.material;
        if (material) {
            if (Array.isArray(material)) {
                material.forEach((material) => disposeMaterial(material));
            }
            else if (material) {
                disposeMaterial(material);
            }
        }
    }
    function deepDispose(object3D) {
        object3D.traverse(dispose);
    }

    var VRMBlendShapeMaterialValueType;
    (function (VRMBlendShapeMaterialValueType) {
        VRMBlendShapeMaterialValueType[VRMBlendShapeMaterialValueType["NUMBER"] = 0] = "NUMBER";
        VRMBlendShapeMaterialValueType[VRMBlendShapeMaterialValueType["VECTOR2"] = 1] = "VECTOR2";
        VRMBlendShapeMaterialValueType[VRMBlendShapeMaterialValueType["VECTOR3"] = 2] = "VECTOR3";
        VRMBlendShapeMaterialValueType[VRMBlendShapeMaterialValueType["VECTOR4"] = 3] = "VECTOR4";
        VRMBlendShapeMaterialValueType[VRMBlendShapeMaterialValueType["COLOR"] = 4] = "COLOR";
    })(VRMBlendShapeMaterialValueType || (VRMBlendShapeMaterialValueType = {}));
    const _v2 = new THREE.Vector2();
    const _v3 = new THREE.Vector3();
    const _v4 = new THREE.Vector4();
    const _color = new THREE.Color();
    // animationMixer の監視対象は、Scene の中に入っている必要がある。
    // そのため、表示オブジェクトではないけれど、Object3D を継承して Scene に投入できるようにする。
    class VRMBlendShapeGroup extends THREE.Object3D {
        constructor(expressionName) {
            super();
            this.weight = 0.0;
            this.isBinary = false;
            this._binds = [];
            this._materialValues = [];
            this.name = `BlendShapeController_${expressionName}`;
            // traverse 時の救済手段として Object3D ではないことを明示しておく
            this.type = 'BlendShapeController';
            // 表示目的のオブジェクトではないので、負荷軽減のために visible を false にしておく。
            // これにより、このインスタンスに対する毎フレームの matrix 自動計算を省略できる。
            this.visible = false;
        }
        addBind(args) {
            // original weight is 0-100 but we want to deal with this value within 0-1
            const weight = args.weight / 100;
            this._binds.push({
                meshes: args.meshes,
                morphTargetIndex: args.morphTargetIndex,
                weight,
            });
        }
        addMaterialValue(args) {
            const material = args.material;
            const propertyName = args.propertyName;
            let value = material[propertyName];
            if (!value) {
                // property has not been found
                return;
            }
            value = args.defaultValue || value;
            let type;
            let defaultValue;
            let targetValue;
            let deltaValue;
            if (value.isVector2) {
                type = VRMBlendShapeMaterialValueType.VECTOR2;
                defaultValue = value.clone();
                targetValue = new THREE.Vector2().fromArray(args.targetValue);
                deltaValue = targetValue.clone().sub(defaultValue);
            }
            else if (value.isVector3) {
                type = VRMBlendShapeMaterialValueType.VECTOR3;
                defaultValue = value.clone();
                targetValue = new THREE.Vector3().fromArray(args.targetValue);
                deltaValue = targetValue.clone().sub(defaultValue);
            }
            else if (value.isVector4) {
                type = VRMBlendShapeMaterialValueType.VECTOR4;
                defaultValue = value.clone();
                // vectorProperty and targetValue index is different from each other
                // exported vrm by UniVRM file is
                //
                // vectorProperty
                // offset = targetValue[0], targetValue[1]
                // tiling = targetValue[2], targetValue[3]
                //
                // targetValue
                // offset = targetValue[2], targetValue[3]
                // tiling = targetValue[0], targetValue[1]
                targetValue = new THREE.Vector4().fromArray([
                    args.targetValue[2],
                    args.targetValue[3],
                    args.targetValue[0],
                    args.targetValue[1],
                ]);
                deltaValue = targetValue.clone().sub(defaultValue);
            }
            else if (value.isColor) {
                type = VRMBlendShapeMaterialValueType.COLOR;
                defaultValue = value.clone();
                targetValue = new THREE.Color().fromArray(args.targetValue);
                deltaValue = targetValue.clone().sub(defaultValue);
            }
            else {
                type = VRMBlendShapeMaterialValueType.NUMBER;
                defaultValue = value;
                targetValue = args.targetValue[0];
                deltaValue = targetValue - defaultValue;
            }
            this._materialValues.push({
                material,
                propertyName,
                defaultValue,
                targetValue,
                deltaValue,
                type,
            });
        }
        /**
         * Apply weight to every assigned blend shapes.
         * Should be called via {@link BlendShapeMaster#update}.
         */
        applyWeight() {
            const w = this.isBinary ? (this.weight < 0.5 ? 0.0 : 1.0) : this.weight;
            this._binds.forEach((bind) => {
                bind.meshes.forEach((mesh) => {
                    if (!mesh.morphTargetInfluences) {
                        return;
                    } // TODO: we should kick this at `addBind`
                    mesh.morphTargetInfluences[bind.morphTargetIndex] += w * bind.weight;
                });
            });
            this._materialValues.forEach((materialValue) => {
                const prop = materialValue.material[materialValue.propertyName];
                if (prop === undefined) {
                    return;
                } // TODO: we should kick this at `addMaterialValue`
                if (materialValue.type === VRMBlendShapeMaterialValueType.NUMBER) {
                    const deltaValue = materialValue.deltaValue;
                    materialValue.material[materialValue.propertyName] += deltaValue * w;
                }
                else if (materialValue.type === VRMBlendShapeMaterialValueType.VECTOR2) {
                    const deltaValue = materialValue.deltaValue;
                    materialValue.material[materialValue.propertyName].add(_v2.copy(deltaValue).multiplyScalar(w));
                }
                else if (materialValue.type === VRMBlendShapeMaterialValueType.VECTOR3) {
                    const deltaValue = materialValue.deltaValue;
                    materialValue.material[materialValue.propertyName].add(_v3.copy(deltaValue).multiplyScalar(w));
                }
                else if (materialValue.type === VRMBlendShapeMaterialValueType.VECTOR4) {
                    const deltaValue = materialValue.deltaValue;
                    materialValue.material[materialValue.propertyName].add(_v4.copy(deltaValue).multiplyScalar(w));
                }
                else if (materialValue.type === VRMBlendShapeMaterialValueType.COLOR) {
                    const deltaValue = materialValue.deltaValue;
                    materialValue.material[materialValue.propertyName].add(_color.copy(deltaValue).multiplyScalar(w));
                }
                if (typeof materialValue.material.shouldApplyUniforms === 'boolean') {
                    materialValue.material.shouldApplyUniforms = true;
                }
            });
        }
        /**
         * Clear previously assigned blend shapes.
         */
        clearAppliedWeight() {
            this._binds.forEach((bind) => {
                bind.meshes.forEach((mesh) => {
                    if (!mesh.morphTargetInfluences) {
                        return;
                    } // TODO: we should kick this at `addBind`
                    mesh.morphTargetInfluences[bind.morphTargetIndex] = 0.0;
                });
            });
            this._materialValues.forEach((materialValue) => {
                const prop = materialValue.material[materialValue.propertyName];
                if (prop === undefined) {
                    return;
                } // TODO: we should kick this at `addMaterialValue`
                if (materialValue.type === VRMBlendShapeMaterialValueType.NUMBER) {
                    const defaultValue = materialValue.defaultValue;
                    materialValue.material[materialValue.propertyName] = defaultValue;
                }
                else if (materialValue.type === VRMBlendShapeMaterialValueType.VECTOR2) {
                    const defaultValue = materialValue.defaultValue;
                    materialValue.material[materialValue.propertyName].copy(defaultValue);
                }
                else if (materialValue.type === VRMBlendShapeMaterialValueType.VECTOR3) {
                    const defaultValue = materialValue.defaultValue;
                    materialValue.material[materialValue.propertyName].copy(defaultValue);
                }
                else if (materialValue.type === VRMBlendShapeMaterialValueType.VECTOR4) {
                    const defaultValue = materialValue.defaultValue;
                    materialValue.material[materialValue.propertyName].copy(defaultValue);
                }
                else if (materialValue.type === VRMBlendShapeMaterialValueType.COLOR) {
                    const defaultValue = materialValue.defaultValue;
                    materialValue.material[materialValue.propertyName].copy(defaultValue);
                }
                if (typeof materialValue.material.shouldApplyUniforms === 'boolean') {
                    materialValue.material.shouldApplyUniforms = true;
                }
            });
        }
    }

    // Typedoc does not support export declarations yet
    (function (VRMSchema) {
        (function (BlendShapePresetName) {
            BlendShapePresetName["A"] = "a";
            BlendShapePresetName["Angry"] = "angry";
            BlendShapePresetName["Blink"] = "blink";
            BlendShapePresetName["BlinkL"] = "blink_l";
            BlendShapePresetName["BlinkR"] = "blink_r";
            BlendShapePresetName["E"] = "e";
            BlendShapePresetName["Fun"] = "fun";
            BlendShapePresetName["I"] = "i";
            BlendShapePresetName["Joy"] = "joy";
            BlendShapePresetName["Lookdown"] = "lookdown";
            BlendShapePresetName["Lookleft"] = "lookleft";
            BlendShapePresetName["Lookright"] = "lookright";
            BlendShapePresetName["Lookup"] = "lookup";
            BlendShapePresetName["Neutral"] = "neutral";
            BlendShapePresetName["O"] = "o";
            BlendShapePresetName["Sorrow"] = "sorrow";
            BlendShapePresetName["U"] = "u";
            BlendShapePresetName["Unknown"] = "unknown";
        })(VRMSchema.BlendShapePresetName || (VRMSchema.BlendShapePresetName = {}));
        (function (FirstPersonLookAtTypeName) {
            FirstPersonLookAtTypeName["BlendShape"] = "BlendShape";
            FirstPersonLookAtTypeName["Bone"] = "Bone";
        })(VRMSchema.FirstPersonLookAtTypeName || (VRMSchema.FirstPersonLookAtTypeName = {}));
        (function (HumanoidBoneName) {
            HumanoidBoneName["Chest"] = "chest";
            HumanoidBoneName["Head"] = "head";
            HumanoidBoneName["Hips"] = "hips";
            HumanoidBoneName["Jaw"] = "jaw";
            HumanoidBoneName["LeftEye"] = "leftEye";
            HumanoidBoneName["LeftFoot"] = "leftFoot";
            HumanoidBoneName["LeftHand"] = "leftHand";
            HumanoidBoneName["LeftIndexDistal"] = "leftIndexDistal";
            HumanoidBoneName["LeftIndexIntermediate"] = "leftIndexIntermediate";
            HumanoidBoneName["LeftIndexProximal"] = "leftIndexProximal";
            HumanoidBoneName["LeftLittleDistal"] = "leftLittleDistal";
            HumanoidBoneName["LeftLittleIntermediate"] = "leftLittleIntermediate";
            HumanoidBoneName["LeftLittleProximal"] = "leftLittleProximal";
            HumanoidBoneName["LeftLowerArm"] = "leftLowerArm";
            HumanoidBoneName["LeftLowerLeg"] = "leftLowerLeg";
            HumanoidBoneName["LeftMiddleDistal"] = "leftMiddleDistal";
            HumanoidBoneName["LeftMiddleIntermediate"] = "leftMiddleIntermediate";
            HumanoidBoneName["LeftMiddleProximal"] = "leftMiddleProximal";
            HumanoidBoneName["LeftRingDistal"] = "leftRingDistal";
            HumanoidBoneName["LeftRingIntermediate"] = "leftRingIntermediate";
            HumanoidBoneName["LeftRingProximal"] = "leftRingProximal";
            HumanoidBoneName["LeftShoulder"] = "leftShoulder";
            HumanoidBoneName["LeftThumbDistal"] = "leftThumbDistal";
            HumanoidBoneName["LeftThumbIntermediate"] = "leftThumbIntermediate";
            HumanoidBoneName["LeftThumbProximal"] = "leftThumbProximal";
            HumanoidBoneName["LeftToes"] = "leftToes";
            HumanoidBoneName["LeftUpperArm"] = "leftUpperArm";
            HumanoidBoneName["LeftUpperLeg"] = "leftUpperLeg";
            HumanoidBoneName["Neck"] = "neck";
            HumanoidBoneName["RightEye"] = "rightEye";
            HumanoidBoneName["RightFoot"] = "rightFoot";
            HumanoidBoneName["RightHand"] = "rightHand";
            HumanoidBoneName["RightIndexDistal"] = "rightIndexDistal";
            HumanoidBoneName["RightIndexIntermediate"] = "rightIndexIntermediate";
            HumanoidBoneName["RightIndexProximal"] = "rightIndexProximal";
            HumanoidBoneName["RightLittleDistal"] = "rightLittleDistal";
            HumanoidBoneName["RightLittleIntermediate"] = "rightLittleIntermediate";
            HumanoidBoneName["RightLittleProximal"] = "rightLittleProximal";
            HumanoidBoneName["RightLowerArm"] = "rightLowerArm";
            HumanoidBoneName["RightLowerLeg"] = "rightLowerLeg";
            HumanoidBoneName["RightMiddleDistal"] = "rightMiddleDistal";
            HumanoidBoneName["RightMiddleIntermediate"] = "rightMiddleIntermediate";
            HumanoidBoneName["RightMiddleProximal"] = "rightMiddleProximal";
            HumanoidBoneName["RightRingDistal"] = "rightRingDistal";
            HumanoidBoneName["RightRingIntermediate"] = "rightRingIntermediate";
            HumanoidBoneName["RightRingProximal"] = "rightRingProximal";
            HumanoidBoneName["RightShoulder"] = "rightShoulder";
            HumanoidBoneName["RightThumbDistal"] = "rightThumbDistal";
            HumanoidBoneName["RightThumbIntermediate"] = "rightThumbIntermediate";
            HumanoidBoneName["RightThumbProximal"] = "rightThumbProximal";
            HumanoidBoneName["RightToes"] = "rightToes";
            HumanoidBoneName["RightUpperArm"] = "rightUpperArm";
            HumanoidBoneName["RightUpperLeg"] = "rightUpperLeg";
            HumanoidBoneName["Spine"] = "spine";
            HumanoidBoneName["UpperChest"] = "upperChest";
        })(VRMSchema.HumanoidBoneName || (VRMSchema.HumanoidBoneName = {}));
        (function (MetaAllowedUserName) {
            MetaAllowedUserName["Everyone"] = "Everyone";
            MetaAllowedUserName["ExplicitlyLicensedPerson"] = "ExplicitlyLicensedPerson";
            MetaAllowedUserName["OnlyAuthor"] = "OnlyAuthor";
        })(VRMSchema.MetaAllowedUserName || (VRMSchema.MetaAllowedUserName = {}));
        (function (MetaUssageName) {
            MetaUssageName["Allow"] = "Allow";
            MetaUssageName["Disallow"] = "Disallow";
        })(VRMSchema.MetaUssageName || (VRMSchema.MetaUssageName = {}));
        (function (MetaLicenseName) {
            MetaLicenseName["Cc0"] = "CC0";
            MetaLicenseName["CcBy"] = "CC_BY";
            MetaLicenseName["CcByNc"] = "CC_BY_NC";
            MetaLicenseName["CcByNcNd"] = "CC_BY_NC_ND";
            MetaLicenseName["CcByNcSa"] = "CC_BY_NC_SA";
            MetaLicenseName["CcByNd"] = "CC_BY_ND";
            MetaLicenseName["CcBySa"] = "CC_BY_SA";
            MetaLicenseName["Other"] = "Other";
            MetaLicenseName["RedistributionProhibited"] = "Redistribution_Prohibited";
        })(VRMSchema.MetaLicenseName || (VRMSchema.MetaLicenseName = {}));
    })(exports.VRMSchema || (exports.VRMSchema = {}));

    function extractPrimitivesInternal(gltf, nodeIndex, node) {
        /**
         * Let's list up every possible patterns that parsed gltf nodes with a mesh can have,,,
         *
         * "*" indicates that those meshes should be listed up using this function
         *
         * ### A node with a (mesh, a signle primitive)
         *
         * - `THREE.Mesh`: The only primitive of the mesh *
         *
         * ### A node with a (mesh, multiple primitives)
         *
         * - `THREE.Group`: The root of the mesh
         *   - `THREE.Mesh`: A primitive of the mesh *
         *   - `THREE.Mesh`: A primitive of the mesh (2) *
         *
         * ### A node with a (mesh, multiple primitives) AND (a child with a mesh, a single primitive)
         *
         * - `THREE.Group`: The root of the mesh
         *   - `THREE.Mesh`: A primitive of the mesh *
         *   - `THREE.Mesh`: A primitive of the mesh (2) *
         *   - `THREE.Mesh`: A primitive of a MESH OF THE CHILD
         *
         * ### A node with a (mesh, multiple primitives) AND (a child with a mesh, multiple primitives)
         *
         * - `THREE.Group`: The root of the mesh
         *   - `THREE.Mesh`: A primitive of the mesh *
         *   - `THREE.Mesh`: A primitive of the mesh (2) *
         *   - `THREE.Group`: The root of a MESH OF THE CHILD
         *     - `THREE.Mesh`: A primitive of the mesh of the child
         *     - `THREE.Mesh`: A primitive of the mesh of the child (2)
         *
         * ### A node with a (mesh, multiple primitives) BUT the node is a bone
         *
         * - `THREE.Bone`: The root of the node, as a bone
         *   - `THREE.Group`: The root of the mesh
         *     - `THREE.Mesh`: A primitive of the mesh *
         *     - `THREE.Mesh`: A primitive of the mesh (2) *
         *
         * ### A node with a (mesh, multiple primitives) AND (a child with a mesh, multiple primitives) BUT the node is a bone
         *
         * - `THREE.Bone`: The root of the node, as a bone
         *   - `THREE.Group`: The root of the mesh
         *     - `THREE.Mesh`: A primitive of the mesh *
         *     - `THREE.Mesh`: A primitive of the mesh (2) *
         *   - `THREE.Group`: The root of a MESH OF THE CHILD
         *     - `THREE.Mesh`: A primitive of the mesh of the child
         *     - `THREE.Mesh`: A primitive of the mesh of the child (2)
         *
         * ...I will take a strategy that traverses the root of the node and take first (primitiveCount) meshes.
         */
        // Make sure that the node has a mesh
        const schemaNode = gltf.parser.json.nodes[nodeIndex];
        const meshIndex = schemaNode.mesh;
        if (meshIndex == null) {
            return null;
        }
        // How many primitives the mesh has?
        const schemaMesh = gltf.parser.json.meshes[meshIndex];
        const primitiveCount = schemaMesh.primitives.length;
        // Traverse the node and take first (primitiveCount) meshes
        const primitives = [];
        node.traverse((object) => {
            if (primitives.length < primitiveCount) {
                if (object.isMesh) {
                    primitives.push(object);
                }
            }
        });
        return primitives;
    }
    /**
     * Extract primitives ( `THREE.Mesh[]` ) of a node from a loaded GLTF.
     * The main purpose of this function is to distinguish primitives and children from a node that has both meshes and children.
     *
     * It utilizes the behavior that GLTFLoader adds mesh primitives to the node object ( `THREE.Group` ) first then adds its children.
     *
     * @param gltf A GLTF object taken from GLTFLoader
     * @param nodeIndex The index of the node
     */
    function gltfExtractPrimitivesFromNode(gltf, nodeIndex) {
        return __awaiter(this, void 0, void 0, function* () {
            const node = yield gltf.parser.getDependency('node', nodeIndex);
            return extractPrimitivesInternal(gltf, nodeIndex, node);
        });
    }
    /**
     * Extract primitives ( `THREE.Mesh[]` ) of nodes from a loaded GLTF.
     * See {@link gltfExtractPrimitivesFromNode} for more details.
     *
     * It returns a map from node index to extraction result.
     * If a node does not have a mesh, the entry for the node will not be put in the returning map.
     *
     * @param gltf A GLTF object taken from GLTFLoader
     */
    function gltfExtractPrimitivesFromNodes(gltf) {
        return __awaiter(this, void 0, void 0, function* () {
            const nodes = yield gltf.parser.getDependencies('node');
            const map = new Map();
            nodes.forEach((node, index) => {
                const result = extractPrimitivesInternal(gltf, index, node);
                if (result != null) {
                    map.set(index, result);
                }
            });
            return map;
        });
    }

    function renameMaterialProperty(name) {
        if (name[0] !== '_') {
            console.warn(`renameMaterialProperty: Given property name "${name}" might be invalid`);
            return name;
        }
        name = name.substring(1);
        if (!/[A-Z]/.test(name[0])) {
            console.warn(`renameMaterialProperty: Given property name "${name}" might be invalid`);
            return name;
        }
        return name[0].toLowerCase() + name.substring(1);
    }

    /**
     * Clamp an input number within [ `0.0` - `1.0` ].
     *
     * @param value The input value
     */
    function saturate(value) {
        return Math.max(Math.min(value, 1.0), 0.0);
    }
    const _position = new THREE.Vector3();
    const _scale = new THREE.Vector3();
    new THREE.Quaternion();
    /**
     * Extract world rotation of an object from its world space matrix, in cheaper way.
     *
     * @param object The object
     * @param out Target vector
     */
    function getWorldQuaternionLite(object, out) {
        object.matrixWorld.decompose(_position, out, _scale);
        return out;
    }

    class VRMBlendShapeProxy {
        /**
         * Create a new VRMBlendShape.
         */
        constructor() {
            /**
             * List of registered blend shape.
             */
            this._blendShapeGroups = {};
            /**
             * A map from [[VRMSchema.BlendShapePresetName]] to its actual blend shape name.
             */
            this._blendShapePresetMap = {};
            /**
             * A list of name of unknown blend shapes.
             */
            this._unknownGroupNames = [];
            // do nothing
        }
        /**
         * List of name of registered blend shape group.
         */
        get expressions() {
            return Object.keys(this._blendShapeGroups);
        }
        /**
         * A map from [[VRMSchema.BlendShapePresetName]] to its actual blend shape name.
         */
        get blendShapePresetMap() {
            return this._blendShapePresetMap;
        }
        /**
         * A list of name of unknown blend shapes.
         */
        get unknownGroupNames() {
            return this._unknownGroupNames;
        }
        /**
         * Return registered blend shape group.
         *
         * @param name Name of the blend shape group
         */
        getBlendShapeGroup(name) {
            const presetName = this._blendShapePresetMap[name];
            const controller = presetName ? this._blendShapeGroups[presetName] : this._blendShapeGroups[name];
            if (!controller) {
                console.warn(`no blend shape found by ${name}`);
                return undefined;
            }
            return controller;
        }
        /**
         * Register a blend shape group.
         *
         * @param name Name of the blend shape gorup
         * @param controller VRMBlendShapeController that describes the blend shape group
         */
        registerBlendShapeGroup(name, presetName, controller) {
            this._blendShapeGroups[name] = controller;
            if (presetName) {
                this._blendShapePresetMap[presetName] = name;
            }
            else {
                this._unknownGroupNames.push(name);
            }
        }
        /**
         * Get current weight of specified blend shape group.
         *
         * @param name Name of the blend shape group
         */
        getValue(name) {
            var _a;
            const controller = this.getBlendShapeGroup(name);
            return (_a = controller === null || controller === void 0 ? void 0 : controller.weight) !== null && _a !== void 0 ? _a : null;
        }
        /**
         * Set a weight to specified blend shape group.
         *
         * @param name Name of the blend shape group
         * @param weight Weight
         */
        setValue(name, weight) {
            const controller = this.getBlendShapeGroup(name);
            if (controller) {
                controller.weight = saturate(weight);
            }
        }
        /**
         * Get a track name of specified blend shape group.
         * This track name is needed to manipulate its blend shape group via keyframe animations.
         *
         * @example Manipulate a blend shape group using keyframe animation
         * ```js
         * const trackName = vrm.blendShapeProxy.getBlendShapeTrackName( THREE.VRMSchema.BlendShapePresetName.Blink );
         * const track = new THREE.NumberKeyframeTrack(
         *   name,
         *   [ 0.0, 0.5, 1.0 ], // times
         *   [ 0.0, 1.0, 0.0 ] // values
         * );
         *
         * const clip = new THREE.AnimationClip(
         *   'blink', // name
         *   1.0, // duration
         *   [ track ] // tracks
         * );
         *
         * const mixer = new THREE.AnimationMixer( vrm.scene );
         * const action = mixer.clipAction( clip );
         * action.play();
         * ```
         *
         * @param name Name of the blend shape group
         */
        getBlendShapeTrackName(name) {
            const controller = this.getBlendShapeGroup(name);
            return controller ? `${controller.name}.weight` : null;
        }
        /**
         * Update every blend shape groups.
         */
        update() {
            Object.keys(this._blendShapeGroups).forEach((name) => {
                const controller = this._blendShapeGroups[name];
                controller.clearAppliedWeight();
            });
            Object.keys(this._blendShapeGroups).forEach((name) => {
                const controller = this._blendShapeGroups[name];
                controller.applyWeight();
            });
        }
    }

    /**
     * An importer that imports a [[VRMBlendShape]] from a VRM extension of a GLTF.
     */
    class VRMBlendShapeImporter {
        /**
         * Import a [[VRMBlendShape]] from a VRM.
         *
         * @param gltf A parsed result of GLTF taken from GLTFLoader
         */
        import(gltf) {
            var _a;
            return __awaiter(this, void 0, void 0, function* () {
                const vrmExt = (_a = gltf.parser.json.extensions) === null || _a === void 0 ? void 0 : _a.VRM;
                if (!vrmExt) {
                    return null;
                }
                const schemaBlendShape = vrmExt.blendShapeMaster;
                if (!schemaBlendShape) {
                    return null;
                }
                const blendShape = new VRMBlendShapeProxy();
                const blendShapeGroups = schemaBlendShape.blendShapeGroups;
                if (!blendShapeGroups) {
                    return blendShape;
                }
                const blendShapePresetMap = {};
                yield Promise.all(blendShapeGroups.map((schemaGroup) => __awaiter(this, void 0, void 0, function* () {
                    const name = schemaGroup.name;
                    if (name === undefined) {
                        console.warn('VRMBlendShapeImporter: One of blendShapeGroups has no name');
                        return;
                    }
                    let presetName;
                    if (schemaGroup.presetName &&
                        schemaGroup.presetName !== exports.VRMSchema.BlendShapePresetName.Unknown &&
                        !blendShapePresetMap[schemaGroup.presetName]) {
                        presetName = schemaGroup.presetName;
                        blendShapePresetMap[schemaGroup.presetName] = name;
                    }
                    const group = new VRMBlendShapeGroup(name);
                    gltf.scene.add(group);
                    group.isBinary = schemaGroup.isBinary || false;
                    if (schemaGroup.binds) {
                        schemaGroup.binds.forEach((bind) => __awaiter(this, void 0, void 0, function* () {
                            if (bind.mesh === undefined || bind.index === undefined) {
                                return;
                            }
                            const nodesUsingMesh = [];
                            gltf.parser.json.nodes.forEach((node, i) => {
                                if (node.mesh === bind.mesh) {
                                    nodesUsingMesh.push(i);
                                }
                            });
                            const morphTargetIndex = bind.index;
                            yield Promise.all(nodesUsingMesh.map((nodeIndex) => __awaiter(this, void 0, void 0, function* () {
                                var _b;
                                const primitives = (yield gltfExtractPrimitivesFromNode(gltf, nodeIndex));
                                // check if the mesh has the target morph target
                                if (!primitives.every((primitive) => Array.isArray(primitive.morphTargetInfluences) &&
                                    morphTargetIndex < primitive.morphTargetInfluences.length)) {
                                    console.warn(`VRMBlendShapeImporter: ${schemaGroup.name} attempts to index ${morphTargetIndex}th morph but not found.`);
                                    return;
                                }
                                group.addBind({
                                    meshes: primitives,
                                    morphTargetIndex,
                                    weight: (_b = bind.weight) !== null && _b !== void 0 ? _b : 100,
                                });
                            })));
                        }));
                    }
                    const materialValues = schemaGroup.materialValues;
                    if (materialValues) {
                        materialValues.forEach((materialValue) => {
                            if (materialValue.materialName === undefined ||
                                materialValue.propertyName === undefined ||
                                materialValue.targetValue === undefined) {
                                return;
                            }
                            const materials = [];
                            gltf.scene.traverse((object) => {
                                if (object.material) {
                                    const material = object.material;
                                    if (Array.isArray(material)) {
                                        materials.push(...material.filter((mtl) => mtl.name === materialValue.materialName && materials.indexOf(mtl) === -1));
                                    }
                                    else if (material.name === materialValue.materialName && materials.indexOf(material) === -1) {
                                        materials.push(material);
                                    }
                                }
                            });
                            materials.forEach((material) => {
                                group.addMaterialValue({
                                    material,
                                    propertyName: renameMaterialProperty(materialValue.propertyName),
                                    targetValue: materialValue.targetValue,
                                });
                            });
                        });
                    }
                    blendShape.registerBlendShapeGroup(name, presetName, group);
                })));
                return blendShape;
            });
        }
    }

    const VECTOR3_FRONT = Object.freeze(new THREE.Vector3(0.0, 0.0, -1.0));
    const _quat = new THREE.Quaternion();
    var FirstPersonFlag;
    (function (FirstPersonFlag) {
        FirstPersonFlag[FirstPersonFlag["Auto"] = 0] = "Auto";
        FirstPersonFlag[FirstPersonFlag["Both"] = 1] = "Both";
        FirstPersonFlag[FirstPersonFlag["ThirdPersonOnly"] = 2] = "ThirdPersonOnly";
        FirstPersonFlag[FirstPersonFlag["FirstPersonOnly"] = 3] = "FirstPersonOnly";
    })(FirstPersonFlag || (FirstPersonFlag = {}));
    /**
     * This class represents a single [`meshAnnotation`](https://github.com/vrm-c/UniVRM/blob/master/specification/0.0/schema/vrm.firstperson.meshannotation.schema.json) entry.
     * Each mesh will be assigned to specified layer when you call [[VRMFirstPerson.setup]].
     */
    class VRMRendererFirstPersonFlags {
        /**
         * Create a new mesh annotation.
         *
         * @param firstPersonFlag A [[FirstPersonFlag]] of the annotation entry
         * @param node A node of the annotation entry.
         */
        constructor(firstPersonFlag, primitives) {
            this.firstPersonFlag = VRMRendererFirstPersonFlags._parseFirstPersonFlag(firstPersonFlag);
            this.primitives = primitives;
        }
        static _parseFirstPersonFlag(firstPersonFlag) {
            switch (firstPersonFlag) {
                case 'Both':
                    return FirstPersonFlag.Both;
                case 'ThirdPersonOnly':
                    return FirstPersonFlag.ThirdPersonOnly;
                case 'FirstPersonOnly':
                    return FirstPersonFlag.FirstPersonOnly;
                default:
                    return FirstPersonFlag.Auto;
            }
        }
    }
    class VRMFirstPerson {
        /**
         * Create a new VRMFirstPerson object.
         *
         * @param firstPersonBone A first person bone
         * @param firstPersonBoneOffset An offset from the specified first person bone
         * @param meshAnnotations A renderer settings. See the description of [[RendererFirstPersonFlags]] for more info
         */
        constructor(firstPersonBone, firstPersonBoneOffset, meshAnnotations) {
            this._meshAnnotations = [];
            this._firstPersonOnlyLayer = VRMFirstPerson._DEFAULT_FIRSTPERSON_ONLY_LAYER;
            this._thirdPersonOnlyLayer = VRMFirstPerson._DEFAULT_THIRDPERSON_ONLY_LAYER;
            this._initialized = false;
            this._firstPersonBone = firstPersonBone;
            this._firstPersonBoneOffset = firstPersonBoneOffset;
            this._meshAnnotations = meshAnnotations;
        }
        get firstPersonBone() {
            return this._firstPersonBone;
        }
        get meshAnnotations() {
            return this._meshAnnotations;
        }
        getFirstPersonWorldDirection(target) {
            return target.copy(VECTOR3_FRONT).applyQuaternion(getWorldQuaternionLite(this._firstPersonBone, _quat));
        }
        /**
         * A camera layer represents `FirstPersonOnly` layer.
         * Note that **you must call [[setup]] first before you use the layer feature** or it does not work properly.
         *
         * The value is [[DEFAULT_FIRSTPERSON_ONLY_LAYER]] by default but you can change the layer by specifying via [[setup]] if you prefer.
         *
         * @see https://vrm.dev/en/univrm/api/univrm_use_firstperson/
         * @see https://threejs.org/docs/#api/en/core/Layers
         */
        get firstPersonOnlyLayer() {
            return this._firstPersonOnlyLayer;
        }
        /**
         * A camera layer represents `ThirdPersonOnly` layer.
         * Note that **you must call [[setup]] first before you use the layer feature** or it does not work properly.
         *
         * The value is [[DEFAULT_THIRDPERSON_ONLY_LAYER]] by default but you can change the layer by specifying via [[setup]] if you prefer.
         *
         * @see https://vrm.dev/en/univrm/api/univrm_use_firstperson/
         * @see https://threejs.org/docs/#api/en/core/Layers
         */
        get thirdPersonOnlyLayer() {
            return this._thirdPersonOnlyLayer;
        }
        getFirstPersonBoneOffset(target) {
            return target.copy(this._firstPersonBoneOffset);
        }
        /**
         * Get current world position of the first person.
         * The position takes [[FirstPersonBone]] and [[FirstPersonOffset]] into account.
         *
         * @param v3 target
         * @returns Current world position of the first person
         */
        getFirstPersonWorldPosition(v3) {
            // UniVRM#VRMFirstPersonEditor
            // var worldOffset = head.localToWorldMatrix.MultiplyPoint(component.FirstPersonOffset);
            const offset = this._firstPersonBoneOffset;
            const v4 = new THREE.Vector4(offset.x, offset.y, offset.z, 1.0);
            v4.applyMatrix4(this._firstPersonBone.matrixWorld);
            return v3.set(v4.x, v4.y, v4.z);
        }
        /**
         * In this method, it assigns layers for every meshes based on mesh annotations.
         * You must call this method first before you use the layer feature.
         *
         * This is an equivalent of [VRMFirstPerson.Setup](https://github.com/vrm-c/UniVRM/blob/master/Assets/VRM/UniVRM/Scripts/FirstPerson/VRMFirstPerson.cs) of the UniVRM.
         *
         * The `cameraLayer` parameter specifies which layer will be assigned for `FirstPersonOnly` / `ThirdPersonOnly`.
         * In UniVRM, we specified those by naming each desired layer as `FIRSTPERSON_ONLY_LAYER` / `THIRDPERSON_ONLY_LAYER`
         * but we are going to specify these layers at here since we are unable to name layers in Three.js.
         *
         * @param cameraLayer Specify which layer will be for `FirstPersonOnly` / `ThirdPersonOnly`.
         */
        setup({ firstPersonOnlyLayer = VRMFirstPerson._DEFAULT_FIRSTPERSON_ONLY_LAYER, thirdPersonOnlyLayer = VRMFirstPerson._DEFAULT_THIRDPERSON_ONLY_LAYER, } = {}) {
            if (this._initialized) {
                return;
            }
            this._initialized = true;
            this._firstPersonOnlyLayer = firstPersonOnlyLayer;
            this._thirdPersonOnlyLayer = thirdPersonOnlyLayer;
            this._meshAnnotations.forEach((item) => {
                if (item.firstPersonFlag === FirstPersonFlag.FirstPersonOnly) {
                    item.primitives.forEach((primitive) => {
                        primitive.layers.set(this._firstPersonOnlyLayer);
                    });
                }
                else if (item.firstPersonFlag === FirstPersonFlag.ThirdPersonOnly) {
                    item.primitives.forEach((primitive) => {
                        primitive.layers.set(this._thirdPersonOnlyLayer);
                    });
                }
                else if (item.firstPersonFlag === FirstPersonFlag.Auto) {
                    this._createHeadlessModel(item.primitives);
                }
            });
        }
        _excludeTriangles(triangles, bws, skinIndex, exclude) {
            let count = 0;
            if (bws != null && bws.length > 0) {
                for (let i = 0; i < triangles.length; i += 3) {
                    const a = triangles[i];
                    const b = triangles[i + 1];
                    const c = triangles[i + 2];
                    const bw0 = bws[a];
                    const skin0 = skinIndex[a];
                    if (bw0[0] > 0 && exclude.includes(skin0[0]))
                        continue;
                    if (bw0[1] > 0 && exclude.includes(skin0[1]))
                        continue;
                    if (bw0[2] > 0 && exclude.includes(skin0[2]))
                        continue;
                    if (bw0[3] > 0 && exclude.includes(skin0[3]))
                        continue;
                    const bw1 = bws[b];
                    const skin1 = skinIndex[b];
                    if (bw1[0] > 0 && exclude.includes(skin1[0]))
                        continue;
                    if (bw1[1] > 0 && exclude.includes(skin1[1]))
                        continue;
                    if (bw1[2] > 0 && exclude.includes(skin1[2]))
                        continue;
                    if (bw1[3] > 0 && exclude.includes(skin1[3]))
                        continue;
                    const bw2 = bws[c];
                    const skin2 = skinIndex[c];
                    if (bw2[0] > 0 && exclude.includes(skin2[0]))
                        continue;
                    if (bw2[1] > 0 && exclude.includes(skin2[1]))
                        continue;
                    if (bw2[2] > 0 && exclude.includes(skin2[2]))
                        continue;
                    if (bw2[3] > 0 && exclude.includes(skin2[3]))
                        continue;
                    triangles[count++] = a;
                    triangles[count++] = b;
                    triangles[count++] = c;
                }
            }
            return count;
        }
        _createErasedMesh(src, erasingBonesIndex) {
            const dst = new THREE.SkinnedMesh(src.geometry.clone(), src.material);
            dst.name = `${src.name}(erase)`;
            dst.frustumCulled = src.frustumCulled;
            dst.layers.set(this._firstPersonOnlyLayer);
            const geometry = dst.geometry;
            const skinIndexAttr = geometry.getAttribute('skinIndex').array;
            const skinIndex = [];
            for (let i = 0; i < skinIndexAttr.length; i += 4) {
                skinIndex.push([skinIndexAttr[i], skinIndexAttr[i + 1], skinIndexAttr[i + 2], skinIndexAttr[i + 3]]);
            }
            const skinWeightAttr = geometry.getAttribute('skinWeight').array;
            const skinWeight = [];
            for (let i = 0; i < skinWeightAttr.length; i += 4) {
                skinWeight.push([skinWeightAttr[i], skinWeightAttr[i + 1], skinWeightAttr[i + 2], skinWeightAttr[i + 3]]);
            }
            const index = geometry.getIndex();
            if (!index) {
                throw new Error("The geometry doesn't have an index buffer");
            }
            const oldTriangles = Array.from(index.array);
            const count = this._excludeTriangles(oldTriangles, skinWeight, skinIndex, erasingBonesIndex);
            const newTriangle = [];
            for (let i = 0; i < count; i++) {
                newTriangle[i] = oldTriangles[i];
            }
            geometry.setIndex(newTriangle);
            // mtoon material includes onBeforeRender. this is unsupported at SkinnedMesh#clone
            if (src.onBeforeRender) {
                dst.onBeforeRender = src.onBeforeRender;
            }
            dst.bind(new THREE.Skeleton(src.skeleton.bones, src.skeleton.boneInverses), new THREE.Matrix4());
            return dst;
        }
        _createHeadlessModelForSkinnedMesh(parent, mesh) {
            const eraseBoneIndexes = [];
            mesh.skeleton.bones.forEach((bone, index) => {
                if (this._isEraseTarget(bone))
                    eraseBoneIndexes.push(index);
            });
            // Unlike UniVRM we don't copy mesh if no invisible bone was found
            if (!eraseBoneIndexes.length) {
                mesh.layers.enable(this._thirdPersonOnlyLayer);
                mesh.layers.enable(this._firstPersonOnlyLayer);
                return;
            }
            mesh.layers.set(this._thirdPersonOnlyLayer);
            const newMesh = this._createErasedMesh(mesh, eraseBoneIndexes);
            parent.add(newMesh);
        }
        _createHeadlessModel(primitives) {
            primitives.forEach((primitive) => {
                if (primitive.type === 'SkinnedMesh') {
                    const skinnedMesh = primitive;
                    this._createHeadlessModelForSkinnedMesh(skinnedMesh.parent, skinnedMesh);
                }
                else {
                    if (this._isEraseTarget(primitive)) {
                        primitive.layers.set(this._thirdPersonOnlyLayer);
                    }
                }
            });
        }
        /**
         * It just checks whether the node or its parent is the first person bone or not.
         * @param bone The target bone
         */
        _isEraseTarget(bone) {
            if (bone === this._firstPersonBone) {
                return true;
            }
            else if (!bone.parent) {
                return false;
            }
            else {
                return this._isEraseTarget(bone.parent);
            }
        }
    }
    /**
     * A default camera layer for `FirstPersonOnly` layer.
     *
     * @see [[getFirstPersonOnlyLayer]]
     */
    VRMFirstPerson._DEFAULT_FIRSTPERSON_ONLY_LAYER = 9;
    /**
     * A default camera layer for `ThirdPersonOnly` layer.
     *
     * @see [[getThirdPersonOnlyLayer]]
     */
    VRMFirstPerson._DEFAULT_THIRDPERSON_ONLY_LAYER = 10;

    /**
     * An importer that imports a [[VRMFirstPerson]] from a VRM extension of a GLTF.
     */
    class VRMFirstPersonImporter {
        /**
         * Import a [[VRMFirstPerson]] from a VRM.
         *
         * @param gltf A parsed result of GLTF taken from GLTFLoader
         * @param humanoid A [[VRMHumanoid]] instance that represents the VRM
         */
        import(gltf, humanoid) {
            var _a;
            return __awaiter(this, void 0, void 0, function* () {
                const vrmExt = (_a = gltf.parser.json.extensions) === null || _a === void 0 ? void 0 : _a.VRM;
                if (!vrmExt) {
                    return null;
                }
                const schemaFirstPerson = vrmExt.firstPerson;
                if (!schemaFirstPerson) {
                    return null;
                }
                const firstPersonBoneIndex = schemaFirstPerson.firstPersonBone;
                let firstPersonBone;
                if (firstPersonBoneIndex === undefined || firstPersonBoneIndex === -1) {
                    firstPersonBone = humanoid.getBoneNode(exports.VRMSchema.HumanoidBoneName.Head);
                }
                else {
                    firstPersonBone = yield gltf.parser.getDependency('node', firstPersonBoneIndex);
                }
                if (!firstPersonBone) {
                    console.warn('VRMFirstPersonImporter: Could not find firstPersonBone of the VRM');
                    return null;
                }
                const firstPersonBoneOffset = schemaFirstPerson.firstPersonBoneOffset
                    ? new THREE.Vector3(schemaFirstPerson.firstPersonBoneOffset.x, schemaFirstPerson.firstPersonBoneOffset.y, -schemaFirstPerson.firstPersonBoneOffset.z)
                    : new THREE.Vector3(0.0, 0.06, 0.0); // fallback, taken from UniVRM implementation
                const meshAnnotations = [];
                const nodePrimitivesMap = yield gltfExtractPrimitivesFromNodes(gltf);
                Array.from(nodePrimitivesMap.entries()).forEach(([nodeIndex, primitives]) => {
                    const schemaNode = gltf.parser.json.nodes[nodeIndex];
                    const flag = schemaFirstPerson.meshAnnotations
                        ? schemaFirstPerson.meshAnnotations.find((a) => a.mesh === schemaNode.mesh)
                        : undefined;
                    meshAnnotations.push(new VRMRendererFirstPersonFlags(flag === null || flag === void 0 ? void 0 : flag.firstPersonFlag, primitives));
                });
                return new VRMFirstPerson(firstPersonBone, firstPersonBoneOffset, meshAnnotations);
            });
        }
    }

    /**
     * A class represents a single `humanBone` of a VRM.
     */
    class VRMHumanBone {
        /**
         * Create a new VRMHumanBone.
         *
         * @param node A [[GLTFNode]] that represents the new bone
         * @param humanLimit A [[VRMHumanLimit]] object that represents properties of the new bone
         */
        constructor(node, humanLimit) {
            this.node = node;
            this.humanLimit = humanLimit;
        }
    }

    /**
     * A compat function for `Quaternion.invert()` / `Quaternion.inverse()`.
     * `Quaternion.invert()` is introduced in r123 and `Quaternion.inverse()` emits a warning.
     * We are going to use this compat for a while.
     * @param target A target quaternion
     */
    function quatInvertCompat(target) {
        if (target.invert) {
            target.invert();
        }
        else {
            target.inverse();
        }
        return target;
    }

    const _v3A = new THREE.Vector3();
    const _quatA = new THREE.Quaternion();
    /**
     * A class represents humanoid of a VRM.
     */
    class VRMHumanoid {
        /**
         * Create a new [[VRMHumanoid]].
         * @param boneArray A [[VRMHumanBoneArray]] contains all the bones of the new humanoid
         * @param humanDescription A [[VRMHumanDescription]] that represents properties of the new humanoid
         */
        constructor(boneArray, humanDescription) {
            /**
             * A [[VRMPose]] that is its default state.
             * Note that it's not compatible with `setPose` and `getPose`, since it contains non-relative values of each local transforms.
             */
            this.restPose = {};
            this.humanBones = this._createHumanBones(boneArray);
            this.humanDescription = humanDescription;
            this.restPose = this.getPose();
        }
        /**
         * Return the current pose of this humanoid as a [[VRMPose]].
         *
         * Each transform is a local transform relative from rest pose (T-pose).
         */
        getPose() {
            const pose = {};
            Object.keys(this.humanBones).forEach((vrmBoneName) => {
                const node = this.getBoneNode(vrmBoneName);
                // Ignore when there are no bone on the VRMHumanoid
                if (!node) {
                    return;
                }
                // When there are two or more bones in a same name, we are not going to overwrite existing one
                if (pose[vrmBoneName]) {
                    return;
                }
                // Take a diff from restPose
                // note that restPose also will use getPose to initialize itself
                _v3A.set(0, 0, 0);
                _quatA.identity();
                const restState = this.restPose[vrmBoneName];
                if (restState === null || restState === void 0 ? void 0 : restState.position) {
                    _v3A.fromArray(restState.position).negate();
                }
                if (restState === null || restState === void 0 ? void 0 : restState.rotation) {
                    quatInvertCompat(_quatA.fromArray(restState.rotation));
                }
                // Get the position / rotation from the node
                _v3A.add(node.position);
                _quatA.premultiply(node.quaternion);
                pose[vrmBoneName] = {
                    position: _v3A.toArray(),
                    rotation: _quatA.toArray(),
                };
            }, {});
            return pose;
        }
        /**
         * Let the humanoid do a specified pose.
         *
         * Each transform have to be a local transform relative from rest pose (T-pose).
         * You can pass what you got from {@link getPose}.
         *
         * @param poseObject A [[VRMPose]] that represents a single pose
         */
        setPose(poseObject) {
            Object.keys(poseObject).forEach((boneName) => {
                const state = poseObject[boneName];
                const node = this.getBoneNode(boneName);
                // Ignore when there are no bone that is defined in the pose on the VRMHumanoid
                if (!node) {
                    return;
                }
                const restState = this.restPose[boneName];
                if (!restState) {
                    return;
                }
                if (state.position) {
                    node.position.fromArray(state.position);
                    if (restState.position) {
                        node.position.add(_v3A.fromArray(restState.position));
                    }
                }
                if (state.rotation) {
                    node.quaternion.fromArray(state.rotation);
                    if (restState.rotation) {
                        node.quaternion.multiply(_quatA.fromArray(restState.rotation));
                    }
                }
            });
        }
        /**
         * Reset the humanoid to its rest pose.
         */
        resetPose() {
            this.setPose({});
        }
        /**
         * Return a bone bound to a specified [[HumanBone]], as a [[VRMHumanBone]].
         *
         * See also: [[VRMHumanoid.getBones]]
         *
         * @param name Name of the bone you want
         */
        getBone(name) {
            return this.humanBones[name][0] || undefined;
        }
        /**
         * Return bones bound to a specified [[HumanBone]], as an array of [[VRMHumanBone]].
         *
         * See also: [[VRMHumanoid.getBone]]
         *
         * @param name Name of the bone you want
         */
        getBones(name) {
            return this.humanBones[name];
        }
        /**
         * Return a bone bound to a specified [[HumanBone]], as a THREE.Object3D.
         *
         * See also: [[VRMHumanoid.getBoneNodes]]
         *
         * @param name Name of the bone you want
         */
        getBoneNode(name) {
            var _a, _b;
            return (_b = (_a = this.humanBones[name][0]) === null || _a === void 0 ? void 0 : _a.node) !== null && _b !== void 0 ? _b : null;
        }
        /**
         * Return bones bound to a specified [[HumanBone]], as an array of THREE.Object3D.
         *
         * See also: [[VRMHumanoid.getBoneNode]]
         *
         * @param name Name of the bone you want
         */
        getBoneNodes(name) {
            return this.humanBones[name].map((bone) => bone.node);
        }
        /**
         * Prepare a [[VRMHumanBones]] from a [[VRMHumanBoneArray]].
         */
        _createHumanBones(boneArray) {
            const bones = Object.values(exports.VRMSchema.HumanoidBoneName).reduce((accum, name) => {
                accum[name] = [];
                return accum;
            }, {});
            boneArray.forEach((bone) => {
                bones[bone.name].push(bone.bone);
            });
            return bones;
        }
    }

    /**
     * An importer that imports a [[VRMHumanoid]] from a VRM extension of a GLTF.
     */
    class VRMHumanoidImporter {
        /**
         * Import a [[VRMHumanoid]] from a VRM.
         *
         * @param gltf A parsed result of GLTF taken from GLTFLoader
         */
        import(gltf) {
            var _a;
            return __awaiter(this, void 0, void 0, function* () {
                const vrmExt = (_a = gltf.parser.json.extensions) === null || _a === void 0 ? void 0 : _a.VRM;
                if (!vrmExt) {
                    return null;
                }
                const schemaHumanoid = vrmExt.humanoid;
                if (!schemaHumanoid) {
                    return null;
                }
                const humanBoneArray = [];
                if (schemaHumanoid.humanBones) {
                    yield Promise.all(schemaHumanoid.humanBones.map((bone) => __awaiter(this, void 0, void 0, function* () {
                        if (!bone.bone || bone.node == null) {
                            return;
                        }
                        const node = yield gltf.parser.getDependency('node', bone.node);
                        humanBoneArray.push({
                            name: bone.bone,
                            bone: new VRMHumanBone(node, {
                                axisLength: bone.axisLength,
                                center: bone.center && new THREE.Vector3(bone.center.x, bone.center.y, bone.center.z),
                                max: bone.max && new THREE.Vector3(bone.max.x, bone.max.y, bone.max.z),
                                min: bone.min && new THREE.Vector3(bone.min.x, bone.min.y, bone.min.z),
                                useDefaultValues: bone.useDefaultValues,
                            }),
                        });
                    })));
                }
                const humanDescription = {
                    armStretch: schemaHumanoid.armStretch,
                    legStretch: schemaHumanoid.legStretch,
                    upperArmTwist: schemaHumanoid.upperArmTwist,
                    lowerArmTwist: schemaHumanoid.lowerArmTwist,
                    upperLegTwist: schemaHumanoid.upperLegTwist,
                    lowerLegTwist: schemaHumanoid.lowerLegTwist,
                    feetSpacing: schemaHumanoid.feetSpacing,
                    hasTranslationDoF: schemaHumanoid.hasTranslationDoF,
                };
                return new VRMHumanoid(humanBoneArray, humanDescription);
            });
        }
    }

    /**
     * Evaluate a hermite spline.
     *
     * @param y0 y on start
     * @param y1 y on end
     * @param t0 delta y on start
     * @param t1 delta y on end
     * @param x input value
     */
    const hermiteSpline = (y0, y1, t0, t1, x) => {
        const xc = x * x * x;
        const xs = x * x;
        const dy = y1 - y0;
        const h01 = -2.0 * xc + 3.0 * xs;
        const h10 = xc - 2.0 * xs + x;
        const h11 = xc - xs;
        return y0 + dy * h01 + t0 * h10 + t1 * h11;
    };
    /**
     * Evaluate an AnimationCurve array. See AnimationCurve class of Unity for its details.
     *
     * See: https://docs.unity3d.com/ja/current/ScriptReference/AnimationCurve.html
     *
     * @param arr An array represents a curve
     * @param x An input value
     */
    const evaluateCurve = (arr, x) => {
        // -- sanity check -----------------------------------------------------------
        if (arr.length < 8) {
            throw new Error('evaluateCurve: Invalid curve detected! (Array length must be 8 at least)');
        }
        if (arr.length % 4 !== 0) {
            throw new Error('evaluateCurve: Invalid curve detected! (Array length must be multiples of 4');
        }
        // -- check range ------------------------------------------------------------
        let outNode;
        for (outNode = 0;; outNode++) {
            if (arr.length <= 4 * outNode) {
                return arr[4 * outNode - 3]; // too further!! assume as "Clamp"
            }
            else if (x <= arr[4 * outNode]) {
                break;
            }
        }
        const inNode = outNode - 1;
        if (inNode < 0) {
            return arr[4 * inNode + 5]; // too behind!! assume as "Clamp"
        }
        // -- calculate local x ------------------------------------------------------
        const x0 = arr[4 * inNode];
        const x1 = arr[4 * outNode];
        const xHermite = (x - x0) / (x1 - x0);
        // -- finally do the hermite spline ------------------------------------------
        const y0 = arr[4 * inNode + 1];
        const y1 = arr[4 * outNode + 1];
        const t0 = arr[4 * inNode + 3];
        const t1 = arr[4 * outNode + 2];
        return hermiteSpline(y0, y1, t0, t1, xHermite);
    };
    /**
     * This is an equivalent of CurveMapper class defined in UniVRM.
     * Will be used for [[VRMLookAtApplyer]]s, to define behavior of LookAt.
     *
     * See: https://github.com/vrm-c/UniVRM/blob/master/Assets/VRM/UniVRM/Scripts/LookAt/CurveMapper.cs
     */
    class VRMCurveMapper {
        /**
         * Create a new [[VRMCurveMapper]].
         *
         * @param xRange The maximum input range
         * @param yRange The maximum output value
         * @param curve An array represents the curve
         */
        constructor(xRange, yRange, curve) {
            /**
             * An array represents the curve. See AnimationCurve class of Unity for its details.
             *
             * See: https://docs.unity3d.com/ja/current/ScriptReference/AnimationCurve.html
             */
            this.curve = [0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0];
            /**
             * The maximum input range of the [[VRMCurveMapper]].
             */
            this.curveXRangeDegree = 90.0;
            /**
             * The maximum output value of the [[VRMCurveMapper]].
             */
            this.curveYRangeDegree = 10.0;
            if (xRange !== undefined) {
                this.curveXRangeDegree = xRange;
            }
            if (yRange !== undefined) {
                this.curveYRangeDegree = yRange;
            }
            if (curve !== undefined) {
                this.curve = curve;
            }
        }
        /**
         * Evaluate an input value and output a mapped value.
         *
         * @param src The input value
         */
        map(src) {
            const clampedSrc = Math.min(Math.max(src, 0.0), this.curveXRangeDegree);
            const x = clampedSrc / this.curveXRangeDegree;
            return this.curveYRangeDegree * evaluateCurve(this.curve, x);
        }
    }

    /**
     * This class is used by [[VRMLookAtHead]], applies look at direction.
     * There are currently two variant of applier: [[VRMLookAtBoneApplyer]] and [[VRMLookAtBlendShapeApplyer]].
     */
    class VRMLookAtApplyer {
    }

    /**
     * This class is used by [[VRMLookAtHead]], applies look at direction to eye blend shapes of a VRM.
     */
    class VRMLookAtBlendShapeApplyer extends VRMLookAtApplyer {
        /**
         * Create a new VRMLookAtBlendShapeApplyer.
         *
         * @param blendShapeProxy A [[VRMBlendShapeProxy]] used by this applier
         * @param curveHorizontal A [[VRMCurveMapper]] used for transverse direction
         * @param curveVerticalDown A [[VRMCurveMapper]] used for down direction
         * @param curveVerticalUp A [[VRMCurveMapper]] used for up direction
         */
        constructor(blendShapeProxy, curveHorizontal, curveVerticalDown, curveVerticalUp) {
            super();
            this.type = exports.VRMSchema.FirstPersonLookAtTypeName.BlendShape;
            this._curveHorizontal = curveHorizontal;
            this._curveVerticalDown = curveVerticalDown;
            this._curveVerticalUp = curveVerticalUp;
            this._blendShapeProxy = blendShapeProxy;
        }
        name() {
            return exports.VRMSchema.FirstPersonLookAtTypeName.BlendShape;
        }
        lookAt(euler) {
            const srcX = euler.x;
            const srcY = euler.y;
            if (srcX < 0.0) {
                this._blendShapeProxy.setValue(exports.VRMSchema.BlendShapePresetName.Lookup, 0.0);
                this._blendShapeProxy.setValue(exports.VRMSchema.BlendShapePresetName.Lookdown, this._curveVerticalDown.map(-srcX));
            }
            else {
                this._blendShapeProxy.setValue(exports.VRMSchema.BlendShapePresetName.Lookdown, 0.0);
                this._blendShapeProxy.setValue(exports.VRMSchema.BlendShapePresetName.Lookup, this._curveVerticalUp.map(srcX));
            }
            if (srcY < 0.0) {
                this._blendShapeProxy.setValue(exports.VRMSchema.BlendShapePresetName.Lookleft, 0.0);
                this._blendShapeProxy.setValue(exports.VRMSchema.BlendShapePresetName.Lookright, this._curveHorizontal.map(-srcY));
            }
            else {
                this._blendShapeProxy.setValue(exports.VRMSchema.BlendShapePresetName.Lookright, 0.0);
                this._blendShapeProxy.setValue(exports.VRMSchema.BlendShapePresetName.Lookleft, this._curveHorizontal.map(srcY));
            }
        }
    }

    const VECTOR3_FRONT$1 = Object.freeze(new THREE.Vector3(0.0, 0.0, -1.0));
    const _v3A$1 = new THREE.Vector3();
    const _v3B = new THREE.Vector3();
    const _v3C = new THREE.Vector3();
    const _quat$1 = new THREE.Quaternion();
    /**
     * A class represents look at of a VRM.
     */
    class VRMLookAtHead {
        /**
         * Create a new VRMLookAtHead.
         *
         * @param firstPerson A [[VRMFirstPerson]] that will be associated with this new VRMLookAtHead
         * @param applyer A [[VRMLookAtApplyer]] that will be associated with this new VRMLookAtHead
         */
        constructor(firstPerson, applyer) {
            /**
             * If this is true, its look at direction will be updated automatically by calling [[VRMLookAtHead.update]] (which is called from [[VRM.update]]).
             *
             * See also: [[VRMLookAtHead.target]]
             */
            this.autoUpdate = true;
            this._euler = new THREE.Euler(0.0, 0.0, 0.0, VRMLookAtHead.EULER_ORDER);
            this.firstPerson = firstPerson;
            this.applyer = applyer;
        }
        /**
         * Get its look at direction in world coordinate.
         *
         * @param target A target `THREE.Vector3`
         */
        getLookAtWorldDirection(target) {
            const rot = getWorldQuaternionLite(this.firstPerson.firstPersonBone, _quat$1);
            return target.copy(VECTOR3_FRONT$1).applyEuler(this._euler).applyQuaternion(rot);
        }
        /**
         * Set its look at position.
         * Note that its result will be instantly overwritten if [[VRMLookAtHead.autoUpdate]] is enabled.
         *
         * @param position A target position
         */
        lookAt(position) {
            this._calcEuler(this._euler, position);
            if (this.applyer) {
                this.applyer.lookAt(this._euler);
            }
        }
        /**
         * Update the VRMLookAtHead.
         * If [[VRMLookAtHead.autoUpdate]] is disabled, it will do nothing.
         *
         * @param delta deltaTime
         */
        update(delta) {
            if (this.target && this.autoUpdate) {
                this.lookAt(this.target.getWorldPosition(_v3A$1));
                if (this.applyer) {
                    this.applyer.lookAt(this._euler);
                }
            }
        }
        _calcEuler(target, position) {
            const headPosition = this.firstPerson.getFirstPersonWorldPosition(_v3B);
            // Look at direction in world coordinate
            const lookAtDir = _v3C.copy(position).sub(headPosition).normalize();
            // Transform the direction into local coordinate from the first person bone
            lookAtDir.applyQuaternion(quatInvertCompat(getWorldQuaternionLite(this.firstPerson.firstPersonBone, _quat$1)));
            // convert the direction into euler
            target.x = Math.atan2(lookAtDir.y, Math.sqrt(lookAtDir.x * lookAtDir.x + lookAtDir.z * lookAtDir.z));
            target.y = Math.atan2(-lookAtDir.x, -lookAtDir.z);
            return target;
        }
    }
    VRMLookAtHead.EULER_ORDER = 'YXZ'; // yaw-pitch-roll

    const _euler = new THREE.Euler(0.0, 0.0, 0.0, VRMLookAtHead.EULER_ORDER);
    /**
     * This class is used by [[VRMLookAtHead]], applies look at direction to eye bones of a VRM.
     */
    class VRMLookAtBoneApplyer extends VRMLookAtApplyer {
        /**
         * Create a new VRMLookAtBoneApplyer.
         *
         * @param humanoid A [[VRMHumanoid]] used by this applier
         * @param curveHorizontalInner A [[VRMCurveMapper]] used for inner transverse direction
         * @param curveHorizontalOuter A [[VRMCurveMapper]] used for outer transverse direction
         * @param curveVerticalDown A [[VRMCurveMapper]] used for down direction
         * @param curveVerticalUp A [[VRMCurveMapper]] used for up direction
         */
        constructor(humanoid, curveHorizontalInner, curveHorizontalOuter, curveVerticalDown, curveVerticalUp) {
            super();
            this.type = exports.VRMSchema.FirstPersonLookAtTypeName.Bone;
            this._curveHorizontalInner = curveHorizontalInner;
            this._curveHorizontalOuter = curveHorizontalOuter;
            this._curveVerticalDown = curveVerticalDown;
            this._curveVerticalUp = curveVerticalUp;
            this._leftEye = humanoid.getBoneNode(exports.VRMSchema.HumanoidBoneName.LeftEye);
            this._rightEye = humanoid.getBoneNode(exports.VRMSchema.HumanoidBoneName.RightEye);
        }
        lookAt(euler) {
            const srcX = euler.x;
            const srcY = euler.y;
            // left
            if (this._leftEye) {
                if (srcX < 0.0) {
                    _euler.x = -this._curveVerticalDown.map(-srcX);
                }
                else {
                    _euler.x = this._curveVerticalUp.map(srcX);
                }
                if (srcY < 0.0) {
                    _euler.y = -this._curveHorizontalInner.map(-srcY);
                }
                else {
                    _euler.y = this._curveHorizontalOuter.map(srcY);
                }
                this._leftEye.quaternion.setFromEuler(_euler);
            }
            // right
            if (this._rightEye) {
                if (srcX < 0.0) {
                    _euler.x = -this._curveVerticalDown.map(-srcX);
                }
                else {
                    _euler.x = this._curveVerticalUp.map(srcX);
                }
                if (srcY < 0.0) {
                    _euler.y = -this._curveHorizontalOuter.map(-srcY);
                }
                else {
                    _euler.y = this._curveHorizontalInner.map(srcY);
                }
                this._rightEye.quaternion.setFromEuler(_euler);
            }
        }
    }

    // THREE.Math has been renamed to THREE.MathUtils since r113.
    // We are going to define the DEG2RAD by ourselves for a while
    // https://github.com/mrdoob/three.js/pull/18270
    const DEG2RAD = Math.PI / 180; // THREE.MathUtils.DEG2RAD;
    /**
     * An importer that imports a [[VRMLookAtHead]] from a VRM extension of a GLTF.
     */
    class VRMLookAtImporter {
        /**
         * Import a [[VRMLookAtHead]] from a VRM.
         *
         * @param gltf A parsed result of GLTF taken from GLTFLoader
         * @param blendShapeProxy A [[VRMBlendShapeProxy]] instance that represents the VRM
         * @param humanoid A [[VRMHumanoid]] instance that represents the VRM
         */
        import(gltf, firstPerson, blendShapeProxy, humanoid) {
            var _a;
            const vrmExt = (_a = gltf.parser.json.extensions) === null || _a === void 0 ? void 0 : _a.VRM;
            if (!vrmExt) {
                return null;
            }
            const schemaFirstPerson = vrmExt.firstPerson;
            if (!schemaFirstPerson) {
                return null;
            }
            const applyer = this._importApplyer(schemaFirstPerson, blendShapeProxy, humanoid);
            return new VRMLookAtHead(firstPerson, applyer || undefined);
        }
        _importApplyer(schemaFirstPerson, blendShapeProxy, humanoid) {
            const lookAtHorizontalInner = schemaFirstPerson.lookAtHorizontalInner;
            const lookAtHorizontalOuter = schemaFirstPerson.lookAtHorizontalOuter;
            const lookAtVerticalDown = schemaFirstPerson.lookAtVerticalDown;
            const lookAtVerticalUp = schemaFirstPerson.lookAtVerticalUp;
            switch (schemaFirstPerson.lookAtTypeName) {
                case exports.VRMSchema.FirstPersonLookAtTypeName.Bone: {
                    if (lookAtHorizontalInner === undefined ||
                        lookAtHorizontalOuter === undefined ||
                        lookAtVerticalDown === undefined ||
                        lookAtVerticalUp === undefined) {
                        return null;
                    }
                    else {
                        return new VRMLookAtBoneApplyer(humanoid, this._importCurveMapperBone(lookAtHorizontalInner), this._importCurveMapperBone(lookAtHorizontalOuter), this._importCurveMapperBone(lookAtVerticalDown), this._importCurveMapperBone(lookAtVerticalUp));
                    }
                }
                case exports.VRMSchema.FirstPersonLookAtTypeName.BlendShape: {
                    if (lookAtHorizontalOuter === undefined || lookAtVerticalDown === undefined || lookAtVerticalUp === undefined) {
                        return null;
                    }
                    else {
                        return new VRMLookAtBlendShapeApplyer(blendShapeProxy, this._importCurveMapperBlendShape(lookAtHorizontalOuter), this._importCurveMapperBlendShape(lookAtVerticalDown), this._importCurveMapperBlendShape(lookAtVerticalUp));
                    }
                }
                default: {
                    return null;
                }
            }
        }
        _importCurveMapperBone(map) {
            return new VRMCurveMapper(typeof map.xRange === 'number' ? DEG2RAD * map.xRange : undefined, typeof map.yRange === 'number' ? DEG2RAD * map.yRange : undefined, map.curve);
        }
        _importCurveMapperBlendShape(map) {
            return new VRMCurveMapper(typeof map.xRange === 'number' ? DEG2RAD * map.xRange : undefined, map.yRange, map.curve);
        }
    }

    const getEncodingComponents = (encoding) => {
        switch (encoding) {
            case THREE.LinearEncoding:
                return ['Linear', '( value )'];
            case THREE.sRGBEncoding:
                return ['sRGB', '( value )'];
            case THREE.RGBEEncoding:
                return ['RGBE', '( value )'];
            case THREE.RGBM7Encoding:
                return ['RGBM', '( value, 7.0 )'];
            case THREE.RGBM16Encoding:
                return ['RGBM', '( value, 16.0 )'];
            case THREE.RGBDEncoding:
                return ['RGBD', '( value, 256.0 )'];
            case THREE.GammaEncoding:
                return ['Gamma', '( value, float( GAMMA_FACTOR ) )'];
            default:
                throw new Error('unsupported encoding: ' + encoding);
        }
    };
    const getTexelDecodingFunction = (functionName, encoding) => {
        const components = getEncodingComponents(encoding);
        return 'vec4 ' + functionName + '( vec4 value ) { return ' + components[0] + 'ToLinear' + components[1] + '; }';
    };

    var vertexShader = "// #define PHONG\n\nvarying vec3 vViewPosition;\n\n#ifndef FLAT_SHADED\n  varying vec3 vNormal;\n#endif\n\n#include <common>\n\n// #include <uv_pars_vertex>\n#if defined( USE_MAP ) || defined( USE_SHADETEXTURE ) || defined( USE_NORMALMAP ) || defined( USE_RECEIVESHADOWTEXTURE ) || defined( USE_SHADINGGRADETEXTURE ) || defined( USE_RIMTEXTURE ) || defined( USE_EMISSIVEMAP ) || defined( USE_OUTLINEWIDTHTEXTURE ) || defined( USE_UVANIMMASKTEXTURE )\n  varying vec2 vUv;\n  uniform vec4 mainTex_ST;\n#endif\n\n#include <uv2_pars_vertex>\n// #include <displacementmap_pars_vertex>\n// #include <envmap_pars_vertex>\n#include <color_pars_vertex>\n#include <fog_pars_vertex>\n#include <morphtarget_pars_vertex>\n#include <skinning_pars_vertex>\n#include <shadowmap_pars_vertex>\n#include <logdepthbuf_pars_vertex>\n#include <clipping_planes_pars_vertex>\n\n#ifdef USE_OUTLINEWIDTHTEXTURE\n  uniform sampler2D outlineWidthTexture;\n#endif\n\nuniform float outlineWidth;\nuniform float outlineScaledMaxDistance;\n\nvoid main() {\n\n  // #include <uv_vertex>\n  #if defined( USE_MAP ) || defined( USE_SHADETEXTURE ) || defined( USE_NORMALMAP ) || defined( USE_RECEIVESHADOWTEXTURE ) || defined( USE_SHADINGGRADETEXTURE ) || defined( USE_RIMTEXTURE ) || defined( USE_EMISSIVEMAP ) || defined( USE_OUTLINEWIDTHTEXTURE ) || defined( USE_UVANIMMASKTEXTURE )\n    vUv = uv;\n    vUv.y = 1.0 - vUv.y; // uv.y is opposite from UniVRM's\n    vUv = mainTex_ST.st + mainTex_ST.pq * vUv;\n    vUv.y = 1.0 - vUv.y; // reverting the previous flip\n  #endif\n\n  #include <uv2_vertex>\n  #include <color_vertex>\n\n  #include <beginnormal_vertex>\n  #include <morphnormal_vertex>\n  #include <skinbase_vertex>\n  #include <skinnormal_vertex>\n\n  // we need this to compute the outline properly\n  objectNormal = normalize( objectNormal );\n\n  #include <defaultnormal_vertex>\n\n  #ifndef FLAT_SHADED // Normal computed with derivatives when FLAT_SHADED\n    vNormal = normalize( transformedNormal );\n  #endif\n\n  #include <begin_vertex>\n\n  #include <morphtarget_vertex>\n  #include <skinning_vertex>\n  // #include <displacementmap_vertex>\n  #include <project_vertex>\n  #include <logdepthbuf_vertex>\n  #include <clipping_planes_vertex>\n\n  vViewPosition = - mvPosition.xyz;\n\n  float outlineTex = 1.0;\n\n  #ifdef OUTLINE\n    #ifdef USE_OUTLINEWIDTHTEXTURE\n      outlineTex = texture2D( outlineWidthTexture, vUv ).r;\n    #endif\n\n    #ifdef OUTLINE_WIDTH_WORLD\n      float worldNormalLength = length( transformedNormal );\n      vec3 outlineOffset = 0.01 * outlineWidth * outlineTex * worldNormalLength * objectNormal;\n      gl_Position = projectionMatrix * modelViewMatrix * vec4( outlineOffset + transformed, 1.0 );\n    #endif\n\n    #ifdef OUTLINE_WIDTH_SCREEN\n      vec3 clipNormal = ( projectionMatrix * modelViewMatrix * vec4( objectNormal, 0.0 ) ).xyz;\n      vec2 projectedNormal = normalize( clipNormal.xy );\n      projectedNormal *= min( gl_Position.w, outlineScaledMaxDistance );\n      projectedNormal.x *= projectionMatrix[ 0 ].x / projectionMatrix[ 1 ].y;\n      gl_Position.xy += 0.01 * outlineWidth * outlineTex * projectedNormal.xy;\n    #endif\n\n    gl_Position.z += 1E-6 * gl_Position.w; // anti-artifact magic\n  #endif\n\n  #include <worldpos_vertex>\n  // #include <envmap_vertex>\n  #include <shadowmap_vertex>\n  #include <fog_vertex>\n\n}";

    var fragmentShader = "// #define PHONG\n\n#ifdef BLENDMODE_CUTOUT\n  uniform float cutoff;\n#endif\n\nuniform vec3 color;\nuniform float colorAlpha;\nuniform vec3 shadeColor;\n#ifdef USE_SHADETEXTURE\n  uniform sampler2D shadeTexture;\n#endif\n\nuniform float receiveShadowRate;\n#ifdef USE_RECEIVESHADOWTEXTURE\n  uniform sampler2D receiveShadowTexture;\n#endif\n\nuniform float shadingGradeRate;\n#ifdef USE_SHADINGGRADETEXTURE\n  uniform sampler2D shadingGradeTexture;\n#endif\n\nuniform float shadeShift;\nuniform float shadeToony;\nuniform float lightColorAttenuation;\nuniform float indirectLightIntensity;\n\n#ifdef USE_RIMTEXTURE\n  uniform sampler2D rimTexture;\n#endif\nuniform vec3 rimColor;\nuniform float rimLightingMix;\nuniform float rimFresnelPower;\nuniform float rimLift;\n\n#ifdef USE_SPHEREADD\n  uniform sampler2D sphereAdd;\n#endif\n\nuniform vec3 emissionColor;\n\nuniform vec3 outlineColor;\nuniform float outlineLightingMix;\n\n#ifdef USE_UVANIMMASKTEXTURE\n  uniform sampler2D uvAnimMaskTexture;\n#endif\n\nuniform float uvAnimOffsetX;\nuniform float uvAnimOffsetY;\nuniform float uvAnimTheta;\n\n#include <common>\n#include <packing>\n#include <dithering_pars_fragment>\n#include <color_pars_fragment>\n\n// #include <uv_pars_fragment>\n#if defined( USE_MAP ) || defined( USE_SHADETEXTURE ) || defined( USE_NORMALMAP ) || defined( USE_RECEIVESHADOWTEXTURE ) || defined( USE_SHADINGGRADETEXTURE ) || defined( USE_RIMTEXTURE ) || defined( USE_EMISSIVEMAP ) || defined( USE_OUTLINEWIDTHTEXTURE ) || defined( USE_UVANIMMASKTEXTURE )\n  varying vec2 vUv;\n#endif\n\n#include <uv2_pars_fragment>\n#include <map_pars_fragment>\n// #include <alphamap_pars_fragment>\n#include <aomap_pars_fragment>\n// #include <lightmap_pars_fragment>\n#include <emissivemap_pars_fragment>\n// #include <envmap_pars_fragment>\n// #include <gradientmap_pars_fragment>\n#include <fog_pars_fragment>\n#include <bsdfs>\n#include <lights_pars_begin>\n\n// #include <lights_phong_pars_fragment>\nvarying vec3 vViewPosition;\n\n#ifndef FLAT_SHADED\n  varying vec3 vNormal;\n#endif\n\n#define Material_LightProbeLOD( material ) (0)\n\n#include <shadowmap_pars_fragment>\n// #include <bumpmap_pars_fragment>\n\n// #include <normalmap_pars_fragment>\n#ifdef USE_NORMALMAP\n\n  uniform sampler2D normalMap;\n  uniform vec2 normalScale;\n\n#endif\n\n#ifdef OBJECTSPACE_NORMALMAP\n\n  uniform mat3 normalMatrix;\n\n#endif\n\n#if ! defined ( USE_TANGENT ) && defined ( TANGENTSPACE_NORMALMAP )\n\n  // Per-Pixel Tangent Space Normal Mapping\n  // http://hacksoflife.blogspot.ch/2009/11/per-pixel-tangent-space-normal-mapping.html\n\n  // three-vrm specific change: it requires `uv` as an input in order to support uv scrolls\n\n  vec3 perturbNormal2Arb( vec2 uv, vec3 eye_pos, vec3 surf_norm, vec3 mapN ) {\n\n    // Workaround for Adreno 3XX dFd*( vec3 ) bug. See #9988\n\n    vec3 q0 = vec3( dFdx( eye_pos.x ), dFdx( eye_pos.y ), dFdx( eye_pos.z ) );\n    vec3 q1 = vec3( dFdy( eye_pos.x ), dFdy( eye_pos.y ), dFdy( eye_pos.z ) );\n    vec2 st0 = dFdx( uv.st );\n    vec2 st1 = dFdy( uv.st );\n\n    float scale = sign( st1.t * st0.s - st0.t * st1.s ); // we do not care about the magnitude\n\n    vec3 S = ( q0 * st1.t - q1 * st0.t ) * scale;\n    vec3 T = ( - q0 * st1.s + q1 * st0.s ) * scale;\n\n    // three-vrm specific change: Workaround for the issue that happens when delta of uv = 0.0\n    // TODO: Is this still required? Or shall I make a PR about it?\n\n    if ( length( S ) == 0.0 || length( T ) == 0.0 ) {\n      return surf_norm;\n    }\n\n    S = normalize( S );\n    T = normalize( T );\n    vec3 N = normalize( surf_norm );\n\n    #ifdef DOUBLE_SIDED\n\n      // Workaround for Adreno GPUs gl_FrontFacing bug. See #15850 and #10331\n\n      bool frontFacing = dot( cross( S, T ), N ) > 0.0;\n\n      mapN.xy *= ( float( frontFacing ) * 2.0 - 1.0 );\n\n    #else\n\n      mapN.xy *= ( float( gl_FrontFacing ) * 2.0 - 1.0 );\n\n    #endif\n\n    mat3 tsn = mat3( S, T, N );\n    return normalize( tsn * mapN );\n\n  }\n\n#endif\n\n// #include <specularmap_pars_fragment>\n#include <logdepthbuf_pars_fragment>\n#include <clipping_planes_pars_fragment>\n\n// == lighting stuff ===========================================================\nfloat getLightIntensity(\n  const in IncidentLight directLight,\n  const in GeometricContext geometry,\n  const in float shadow,\n  const in float shadingGrade\n) {\n  float lightIntensity = dot( geometry.normal, directLight.direction );\n  lightIntensity = 0.5 + 0.5 * lightIntensity;\n  lightIntensity = lightIntensity * shadow;\n  lightIntensity = lightIntensity * shadingGrade;\n  lightIntensity = lightIntensity * 2.0 - 1.0;\n  return shadeToony == 1.0\n    ? step( shadeShift, lightIntensity )\n    : smoothstep( shadeShift, shadeShift + ( 1.0 - shadeToony ), lightIntensity );\n}\n\nvec3 getLighting( const in vec3 lightColor ) {\n  vec3 lighting = lightColor;\n  lighting = mix(\n    lighting,\n    vec3( max( 0.001, max( lighting.x, max( lighting.y, lighting.z ) ) ) ),\n    lightColorAttenuation\n  );\n\n  #ifndef PHYSICALLY_CORRECT_LIGHTS\n    lighting *= PI;\n  #endif\n\n  return lighting;\n}\n\nvec3 getDiffuse(\n  const in vec3 lit,\n  const in vec3 shade,\n  const in float lightIntensity,\n  const in vec3 lighting\n) {\n  #ifdef DEBUG_LITSHADERATE\n    return vec3( BRDF_Diffuse_Lambert( lightIntensity * lighting ) );\n  #endif\n\n  return lighting * BRDF_Diffuse_Lambert( mix( shade, lit, lightIntensity ) );\n}\n\nvec3 calcDirectDiffuse(\n  const in vec2 uv,\n  const in vec3 lit,\n  const in vec3 shade,\n  in GeometricContext geometry,\n  inout ReflectedLight reflectedLight\n) {\n  IncidentLight directLight;\n  vec3 lightingSum = vec3( 0.0 );\n\n  float shadingGrade = 1.0;\n  #ifdef USE_SHADINGGRADETEXTURE\n    shadingGrade = 1.0 - shadingGradeRate * ( 1.0 - texture2D( shadingGradeTexture, uv ).r );\n  #endif\n\n  float receiveShadow = receiveShadowRate;\n  #ifdef USE_RECEIVESHADOWTEXTURE\n    receiveShadow *= texture2D( receiveShadowTexture, uv ).a;\n  #endif\n\n  #if ( NUM_POINT_LIGHTS > 0 )\n    PointLight pointLight;\n\n    #pragma unroll_loop_start\n    for ( int i = 0; i < NUM_POINT_LIGHTS; i ++ ) {\n      pointLight = pointLights[ i ];\n      getPointDirectLightIrradiance( pointLight, geometry, directLight );\n\n      float atten = 1.0;\n      #ifdef USE_SHADOWMAP\n        atten = all( bvec2( pointLight.shadow, directLight.visible ) ) ? getPointShadow( pointShadowMap[ i ], pointLight.shadowMapSize, pointLight.shadowBias, pointLight.shadowRadius, vPointShadowCoord[ i ], pointLight.shadowCameraNear, pointLight.shadowCameraFar ) : 1.0;\n      #endif\n\n      float shadow = 1.0 - receiveShadow * ( 1.0 - ( 0.5 + 0.5 * atten ) );\n      float lightIntensity = getLightIntensity( directLight, geometry, shadow, shadingGrade );\n      vec3 lighting = getLighting( directLight.color );\n      reflectedLight.directDiffuse += getDiffuse( lit, shade, lightIntensity, lighting );\n      lightingSum += lighting;\n    }\n    #pragma unroll_loop_end\n  #endif\n\n  #if ( NUM_SPOT_LIGHTS > 0 )\n    SpotLight spotLight;\n\n    #pragma unroll_loop_start\n    for ( int i = 0; i < NUM_SPOT_LIGHTS; i ++ ) {\n      spotLight = spotLights[ i ];\n      getSpotDirectLightIrradiance( spotLight, geometry, directLight );\n\n      float atten = 1.0;\n      #ifdef USE_SHADOWMAP\n        atten = all( bvec2( spotLight.shadow, directLight.visible ) ) ? getShadow( spotShadowMap[ i ], spotLight.shadowMapSize, spotLight.shadowBias, spotLight.shadowRadius, vSpotShadowCoord[ i ] ) : 1.0;\n      #endif\n\n      float shadow = 1.0 - receiveShadow * ( 1.0 - ( 0.5 + 0.5 * atten ) );\n      float lightIntensity = getLightIntensity( directLight, geometry, shadow, shadingGrade );\n      vec3 lighting = getLighting( directLight.color );\n      reflectedLight.directDiffuse += getDiffuse( lit, shade, lightIntensity, lighting );\n      lightingSum += lighting;\n    }\n    #pragma unroll_loop_end\n  #endif\n\n  #if ( NUM_DIR_LIGHTS > 0 )\n    DirectionalLight directionalLight;\n\n    #pragma unroll_loop_start\n    for ( int i = 0; i < NUM_DIR_LIGHTS; i ++ ) {\n      directionalLight = directionalLights[ i ];\n      getDirectionalDirectLightIrradiance( directionalLight, geometry, directLight );\n\n      float atten = 1.0;\n      #ifdef USE_SHADOWMAP\n        atten = all( bvec2( directionalLight.shadow, directLight.visible ) ) ? getShadow( directionalShadowMap[ i ], directionalLight.shadowMapSize, directionalLight.shadowBias, directionalLight.shadowRadius, vDirectionalShadowCoord[ i ] ) : 1.0;\n      #endif\n\n      float shadow = 1.0 - receiveShadow * ( 1.0 - ( 0.5 + 0.5 * atten ) );\n      float lightIntensity = getLightIntensity( directLight, geometry, shadow, shadingGrade );\n      vec3 lighting = getLighting( directLight.color );\n      reflectedLight.directDiffuse += getDiffuse( lit, shade, lightIntensity, lighting );\n      lightingSum += lighting;\n    }\n    #pragma unroll_loop_end\n  #endif\n\n  return lightingSum;\n}\n\n// == post correction ==========================================================\nvoid postCorrection() {\n  #include <tonemapping_fragment>\n  #include <encodings_fragment>\n  #include <fog_fragment>\n  #include <premultiplied_alpha_fragment>\n  #include <dithering_fragment>\n}\n\n// == main procedure ===========================================================\nvoid main() {\n  #include <clipping_planes_fragment>\n\n  vec2 uv = vec2(0.5, 0.5);\n\n  #if defined( USE_MAP ) || defined( USE_SHADETEXTURE ) || defined( USE_NORMALMAP ) || defined( USE_RECEIVESHADOWTEXTURE ) || defined( USE_SHADINGGRADETEXTURE ) || defined( USE_RIMTEXTURE ) || defined( USE_EMISSIVEMAP ) || defined( USE_OUTLINEWIDTHTEXTURE ) || defined( USE_UVANIMMASKTEXTURE )\n    uv = vUv;\n\n    float uvAnimMask = 1.0;\n    #ifdef USE_UVANIMMASKTEXTURE\n      uvAnimMask = texture2D( uvAnimMaskTexture, uv ).x;\n    #endif\n\n    uv = uv + vec2( uvAnimOffsetX, uvAnimOffsetY ) * uvAnimMask;\n    float uvRotCos = cos( uvAnimTheta * uvAnimMask );\n    float uvRotSin = sin( uvAnimTheta * uvAnimMask );\n    uv = mat2( uvRotCos, uvRotSin, -uvRotSin, uvRotCos ) * ( uv - 0.5 ) + 0.5;\n  #endif\n\n  #ifdef DEBUG_UV\n    gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );\n    #if defined( USE_MAP ) || defined( USE_SHADETEXTURE ) || defined( USE_NORMALMAP ) || defined( USE_RECEIVESHADOWTEXTURE ) || defined( USE_SHADINGGRADETEXTURE ) || defined( USE_RIMTEXTURE ) || defined( USE_EMISSIVEMAP ) || defined( USE_OUTLINEWIDTHTEXTURE ) || defined( USE_UVANIMMASKTEXTURE )\n      gl_FragColor = vec4( uv, 0.0, 1.0 );\n    #endif\n    return;\n  #endif\n\n  vec4 diffuseColor = vec4( color, colorAlpha );\n  ReflectedLight reflectedLight = ReflectedLight( vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ) );\n  vec3 totalEmissiveRadiance = emissionColor;\n\n  #include <logdepthbuf_fragment>\n\n  // #include <map_fragment>\n  #ifdef USE_MAP\n    diffuseColor *= mapTexelToLinear( texture2D( map, uv ) );\n  #endif\n\n  #include <color_fragment>\n  // #include <alphamap_fragment>\n\n  // -- MToon: alpha -----------------------------------------------------------\n  // #include <alphatest_fragment>\n  #ifdef BLENDMODE_CUTOUT\n    if ( diffuseColor.a <= cutoff ) { discard; }\n    diffuseColor.a = 1.0;\n  #endif\n\n  #ifdef BLENDMODE_OPAQUE\n    diffuseColor.a = 1.0;\n  #endif\n\n  #if defined( OUTLINE ) && defined( OUTLINE_COLOR_FIXED ) // omitting DebugMode\n    gl_FragColor = vec4( outlineColor, diffuseColor.a );\n    postCorrection();\n    return;\n  #endif\n\n  // #include <specularmap_fragment>\n  #include <normal_fragment_begin>\n\n  #ifdef OUTLINE\n    normal *= -1.0;\n  #endif\n\n  // #include <normal_fragment_maps>\n\n  #ifdef OBJECTSPACE_NORMALMAP\n\n    normal = texture2D( normalMap, uv ).xyz * 2.0 - 1.0; // overrides both flatShading and attribute normals\n\n    #ifdef FLIP_SIDED\n\n      normal = - normal;\n\n    #endif\n\n    #ifdef DOUBLE_SIDED\n\n      normal = normal * ( float( gl_FrontFacing ) * 2.0 - 1.0 );\n\n    #endif\n\n    normal = normalize( normalMatrix * normal );\n\n  #elif defined( TANGENTSPACE_NORMALMAP )\n\n    vec3 mapN = texture2D( normalMap, uv ).xyz * 2.0 - 1.0;\n    mapN.xy *= normalScale;\n\n    #ifdef USE_TANGENT\n\n      normal = normalize( vTBN * mapN );\n\n    #else\n\n      normal = perturbNormal2Arb( uv, -vViewPosition, normal, mapN );\n\n    #endif\n\n  #endif\n\n  // #include <emissivemap_fragment>\n  #ifdef USE_EMISSIVEMAP\n    totalEmissiveRadiance *= emissiveMapTexelToLinear( texture2D( emissiveMap, uv ) ).rgb;\n  #endif\n\n  #ifdef DEBUG_NORMAL\n    gl_FragColor = vec4( 0.5 + 0.5 * normal, 1.0 );\n    return;\n  #endif\n\n  // -- MToon: lighting --------------------------------------------------------\n  // accumulation\n  // #include <lights_phong_fragment>\n  // #include <lights_fragment_begin>\n  vec3 lit = diffuseColor.rgb;\n  vec3 shade = shadeColor;\n  #ifdef USE_SHADETEXTURE\n    shade *= shadeTextureTexelToLinear( texture2D( shadeTexture, uv ) ).rgb;\n  #endif\n\n  GeometricContext geometry;\n\n  geometry.position = - vViewPosition;\n  geometry.normal = normal;\n  geometry.viewDir = normalize( vViewPosition );\n\n  vec3 lighting = calcDirectDiffuse( uv, diffuseColor.rgb, shade, geometry, reflectedLight );\n\n  vec3 irradiance = getAmbientLightIrradiance( ambientLightColor );\n  #if ( NUM_HEMI_LIGHTS > 0 )\n    #pragma unroll_loop_start\n    for ( int i = 0; i < NUM_HEMI_LIGHTS; i ++ ) {\n      irradiance += getHemisphereLightIrradiance( hemisphereLights[ i ], geometry );\n    }\n    #pragma unroll_loop_end\n  #endif\n\n  // #include <lights_fragment_maps>\n  #ifdef USE_LIGHTMAP\n    vec3 lightMapIrradiance = texture2D( lightMap, vUv2 ).rgb * lightMapIntensity;\n    #ifndef PHYSICALLY_CORRECT_LIGHTS\n      lightMapIrradiance *= PI; // factor of PI should not be present; included here to prevent breakage\n    #endif\n    irradiance += lightMapIrradiance;\n  #endif\n\n  // #include <lights_fragment_end>\n  reflectedLight.indirectDiffuse += indirectLightIntensity * irradiance * BRDF_Diffuse_Lambert( lit );\n\n  // modulation\n  #include <aomap_fragment>\n\n  vec3 col = reflectedLight.directDiffuse + reflectedLight.indirectDiffuse;\n\n  // The \"comment out if you want to PBR absolutely\" line\n  col = min(col, lit);\n\n  #if defined( OUTLINE ) && defined( OUTLINE_COLOR_MIXED )\n    gl_FragColor = vec4(\n      outlineColor.rgb * mix( vec3( 1.0 ), col, outlineLightingMix ),\n      diffuseColor.a\n    );\n    postCorrection();\n    return;\n  #endif\n\n  #ifdef DEBUG_LITSHADERATE\n    gl_FragColor = vec4( col, diffuseColor.a );\n    postCorrection();\n    return;\n  #endif\n\n  // -- MToon: parametric rim lighting -----------------------------------------\n  vec3 viewDir = normalize( vViewPosition );\n  vec3 rimMix = mix(vec3(1.0), lighting + indirectLightIntensity * irradiance, rimLightingMix);\n  vec3 rim = rimColor * pow( saturate( 1.0 - dot( viewDir, normal ) + rimLift ), rimFresnelPower );\n  #ifdef USE_RIMTEXTURE\n    rim *= rimTextureTexelToLinear( texture2D( rimTexture, uv ) ).rgb;\n  #endif\n  col += rim;\n\n  // -- MToon: additive matcap -------------------------------------------------\n  #ifdef USE_SPHEREADD\n    {\n      vec3 x = normalize( vec3( viewDir.z, 0.0, -viewDir.x ) );\n      vec3 y = cross( viewDir, x ); // guaranteed to be normalized\n      vec2 sphereUv = 0.5 + 0.5 * vec2( dot( x, normal ), -dot( y, normal ) );\n      vec3 matcap = sphereAddTexelToLinear( texture2D( sphereAdd, sphereUv ) ).xyz;\n      col += matcap;\n    }\n  #endif\n\n  // -- MToon: Emission --------------------------------------------------------\n  col += totalEmissiveRadiance;\n\n  // #include <envmap_fragment>\n\n  // -- Almost done! -----------------------------------------------------------\n  gl_FragColor = vec4( col, diffuseColor.a );\n  postCorrection();\n}";

    /* tslint:disable:member-ordering */
    const TAU = 2.0 * Math.PI;
    (function (MToonMaterialCullMode) {
        MToonMaterialCullMode[MToonMaterialCullMode["Off"] = 0] = "Off";
        MToonMaterialCullMode[MToonMaterialCullMode["Front"] = 1] = "Front";
        MToonMaterialCullMode[MToonMaterialCullMode["Back"] = 2] = "Back";
    })(exports.MToonMaterialCullMode || (exports.MToonMaterialCullMode = {}));
    (function (MToonMaterialDebugMode) {
        MToonMaterialDebugMode[MToonMaterialDebugMode["None"] = 0] = "None";
        MToonMaterialDebugMode[MToonMaterialDebugMode["Normal"] = 1] = "Normal";
        MToonMaterialDebugMode[MToonMaterialDebugMode["LitShadeRate"] = 2] = "LitShadeRate";
        MToonMaterialDebugMode[MToonMaterialDebugMode["UV"] = 3] = "UV";
    })(exports.MToonMaterialDebugMode || (exports.MToonMaterialDebugMode = {}));
    (function (MToonMaterialOutlineColorMode) {
        MToonMaterialOutlineColorMode[MToonMaterialOutlineColorMode["FixedColor"] = 0] = "FixedColor";
        MToonMaterialOutlineColorMode[MToonMaterialOutlineColorMode["MixedLighting"] = 1] = "MixedLighting";
    })(exports.MToonMaterialOutlineColorMode || (exports.MToonMaterialOutlineColorMode = {}));
    (function (MToonMaterialOutlineWidthMode) {
        MToonMaterialOutlineWidthMode[MToonMaterialOutlineWidthMode["None"] = 0] = "None";
        MToonMaterialOutlineWidthMode[MToonMaterialOutlineWidthMode["WorldCoordinates"] = 1] = "WorldCoordinates";
        MToonMaterialOutlineWidthMode[MToonMaterialOutlineWidthMode["ScreenCoordinates"] = 2] = "ScreenCoordinates";
    })(exports.MToonMaterialOutlineWidthMode || (exports.MToonMaterialOutlineWidthMode = {}));
    (function (MToonMaterialRenderMode) {
        MToonMaterialRenderMode[MToonMaterialRenderMode["Opaque"] = 0] = "Opaque";
        MToonMaterialRenderMode[MToonMaterialRenderMode["Cutout"] = 1] = "Cutout";
        MToonMaterialRenderMode[MToonMaterialRenderMode["Transparent"] = 2] = "Transparent";
        MToonMaterialRenderMode[MToonMaterialRenderMode["TransparentWithZWrite"] = 3] = "TransparentWithZWrite";
    })(exports.MToonMaterialRenderMode || (exports.MToonMaterialRenderMode = {}));
    /**
     * MToon is a material specification that has various features.
     * The spec and implementation are originally founded for Unity engine and this is a port of the material.
     *
     * See: https://github.com/Santarh/MToon
     */
    class MToonMaterial extends THREE.ShaderMaterial {
        constructor(parameters = {}) {
            super();
            /**
             * Readonly boolean that indicates this is a [[MToonMaterial]].
             */
            this.isMToonMaterial = true;
            this.cutoff = 0.5; // _Cutoff
            this.color = new THREE.Vector4(1.0, 1.0, 1.0, 1.0); // _Color
            this.shadeColor = new THREE.Vector4(0.97, 0.81, 0.86, 1.0); // _ShadeColor
            this.map = null; // _MainTex
            // eslint-disable-next-line @typescript-eslint/naming-convention
            this.mainTex_ST = new THREE.Vector4(0.0, 0.0, 1.0, 1.0); // _MainTex_ST
            this.shadeTexture = null; // _ShadeTexture
            // public shadeTexture_ST = new THREE.Vector4(0.0, 0.0, 1.0, 1.0); // _ShadeTexture_ST (unused)
            this.normalMap = null; // _BumpMap. again, THIS IS _BumpMap
            this.normalMapType = THREE.TangentSpaceNormalMap; // Three.js requires this
            this.normalScale = new THREE.Vector2(1.0, 1.0); // _BumpScale, in Vector2
            // public bumpMap_ST = new THREE.Vector4(0.0, 0.0, 1.0, 1.0); // _BumpMap_ST (unused)
            this.receiveShadowRate = 1.0; // _ReceiveShadowRate
            this.receiveShadowTexture = null; // _ReceiveShadowTexture
            // public receiveShadowTexture_ST = new THREE.Vector4(0.0, 0.0, 1.0, 1.0); // _ReceiveShadowTexture_ST (unused)
            this.shadingGradeRate = 1.0; // _ShadingGradeRate
            this.shadingGradeTexture = null; // _ShadingGradeTexture
            // public shadingGradeTexture_ST = new THREE.Vector4(0.0, 0.0, 1.0, 1.0); // _ShadingGradeTexture_ST (unused)
            this.shadeShift = 0.0; // _ShadeShift
            this.shadeToony = 0.9; // _ShadeToony
            this.lightColorAttenuation = 0.0; // _LightColorAttenuation
            this.indirectLightIntensity = 0.1; // _IndirectLightIntensity
            this.rimTexture = null; // _RimTexture
            this.rimColor = new THREE.Vector4(0.0, 0.0, 0.0, 1.0); // _RimColor
            this.rimLightingMix = 0.0; // _RimLightingMix
            this.rimFresnelPower = 1.0; // _RimFresnelPower
            this.rimLift = 0.0; // _RimLift
            this.sphereAdd = null; // _SphereAdd
            // public sphereAdd_ST = new THREE.Vector4(0.0, 0.0, 1.0, 1.0); // _SphereAdd_ST (unused)
            this.emissionColor = new THREE.Vector4(0.0, 0.0, 0.0, 1.0); // _EmissionColor
            this.emissiveMap = null; // _EmissionMap
            // public emissionMap_ST = new THREE.Vector4(0.0, 0.0, 1.0, 1.0); // _EmissionMap_ST (unused)
            this.outlineWidthTexture = null; // _OutlineWidthTexture
            // public outlineWidthTexture_ST = new THREE.Vector4(0.0, 0.0, 1.0, 1.0); // _OutlineWidthTexture_ST (unused)
            this.outlineWidth = 0.5; // _OutlineWidth
            this.outlineScaledMaxDistance = 1.0; // _OutlineScaledMaxDistance
            this.outlineColor = new THREE.Vector4(0.0, 0.0, 0.0, 1.0); // _OutlineColor
            this.outlineLightingMix = 1.0; // _OutlineLightingMix
            this.uvAnimMaskTexture = null; // _UvAnimMaskTexture
            this.uvAnimScrollX = 0.0; // _UvAnimScrollX
            this.uvAnimScrollY = 0.0; // _UvAnimScrollY
            this.uvAnimRotation = 0.0; // _uvAnimRotation
            this.shouldApplyUniforms = true; // when this is true, applyUniforms effects
            this._debugMode = exports.MToonMaterialDebugMode.None; // _DebugMode
            this._blendMode = exports.MToonMaterialRenderMode.Opaque; // _BlendMode
            this._outlineWidthMode = exports.MToonMaterialOutlineWidthMode.None; // _OutlineWidthMode
            this._outlineColorMode = exports.MToonMaterialOutlineColorMode.FixedColor; // _OutlineColorMode
            this._cullMode = exports.MToonMaterialCullMode.Back; // _CullMode
            this._outlineCullMode = exports.MToonMaterialCullMode.Front; // _OutlineCullMode
            // public srcBlend = 1.0; // _SrcBlend (is not supported)
            // public dstBlend = 0.0; // _DstBlend (is not supported)
            // public zWrite = 1.0; // _ZWrite (will be converted to depthWrite)
            this._isOutline = false;
            this._uvAnimOffsetX = 0.0;
            this._uvAnimOffsetY = 0.0;
            this._uvAnimPhase = 0.0;
            this.encoding = parameters.encoding || THREE.LinearEncoding;
            if (this.encoding !== THREE.LinearEncoding && this.encoding !== THREE.sRGBEncoding) {
                console.warn('The specified color encoding does not work properly with MToonMaterial. You might want to use THREE.sRGBEncoding instead.');
            }
            // == these parameter has no compatibility with this implementation ========
            [
                'mToonVersion',
                'shadeTexture_ST',
                'bumpMap_ST',
                'receiveShadowTexture_ST',
                'shadingGradeTexture_ST',
                'rimTexture_ST',
                'sphereAdd_ST',
                'emissionMap_ST',
                'outlineWidthTexture_ST',
                'uvAnimMaskTexture_ST',
                'srcBlend',
                'dstBlend',
            ].forEach((key) => {
                if (parameters[key] !== undefined) {
                    // console.warn(`THREE.${this.type}: The parameter "${key}" is not supported.`);
                    delete parameters[key];
                }
            });
            // == enabling bunch of stuff ==============================================
            parameters.fog = true;
            parameters.lights = true;
            parameters.clipping = true;
            parameters.skinning = parameters.skinning || false;
            parameters.morphTargets = parameters.morphTargets || false;
            parameters.morphNormals = parameters.morphNormals || false;
            // == uniforms =============================================================
            parameters.uniforms = THREE.UniformsUtils.merge([
                THREE.UniformsLib.common,
                THREE.UniformsLib.normalmap,
                THREE.UniformsLib.emissivemap,
                THREE.UniformsLib.fog,
                THREE.UniformsLib.lights,
                {
                    cutoff: { value: 0.5 },
                    color: { value: new THREE.Color(1.0, 1.0, 1.0) },
                    colorAlpha: { value: 1.0 },
                    shadeColor: { value: new THREE.Color(0.97, 0.81, 0.86) },
                    // eslint-disable-next-line @typescript-eslint/naming-convention
                    mainTex_ST: { value: new THREE.Vector4(0.0, 0.0, 1.0, 1.0) },
                    shadeTexture: { value: null },
                    receiveShadowRate: { value: 1.0 },
                    receiveShadowTexture: { value: null },
                    shadingGradeRate: { value: 1.0 },
                    shadingGradeTexture: { value: null },
                    shadeShift: { value: 0.0 },
                    shadeToony: { value: 0.9 },
                    lightColorAttenuation: { value: 0.0 },
                    indirectLightIntensity: { value: 0.1 },
                    rimTexture: { value: null },
                    rimColor: { value: new THREE.Color(0.0, 0.0, 0.0) },
                    rimLightingMix: { value: 0.0 },
                    rimFresnelPower: { value: 1.0 },
                    rimLift: { value: 0.0 },
                    sphereAdd: { value: null },
                    emissionColor: { value: new THREE.Color(0.0, 0.0, 0.0) },
                    outlineWidthTexture: { value: null },
                    outlineWidth: { value: 0.5 },
                    outlineScaledMaxDistance: { value: 1.0 },
                    outlineColor: { value: new THREE.Color(0.0, 0.0, 0.0) },
                    outlineLightingMix: { value: 1.0 },
                    uvAnimMaskTexture: { value: null },
                    uvAnimOffsetX: { value: 0.0 },
                    uvAnimOffsetY: { value: 0.0 },
                    uvAnimTheta: { value: 0.0 },
                },
            ]);
            // == finally compile the shader program ===================================
            this.setValues(parameters);
            // == update shader stuff ==================================================
            this._updateShaderCode();
            this._applyUniforms();
        }
        get mainTex() {
            return this.map;
        }
        set mainTex(t) {
            this.map = t;
        }
        get bumpMap() {
            return this.normalMap;
        }
        set bumpMap(t) {
            this.normalMap = t;
        }
        /**
         * Getting the `bumpScale` reutrns its x component of `normalScale` (assuming x and y component of `normalScale` are same).
         */
        get bumpScale() {
            return this.normalScale.x;
        }
        /**
         * Setting the `bumpScale` will be convert the value into Vector2 `normalScale` .
         */
        set bumpScale(t) {
            this.normalScale.set(t, t);
        }
        get emissionMap() {
            return this.emissiveMap;
        }
        set emissionMap(t) {
            this.emissiveMap = t;
        }
        get blendMode() {
            return this._blendMode;
        }
        set blendMode(m) {
            this._blendMode = m;
            this.depthWrite = this._blendMode !== exports.MToonMaterialRenderMode.Transparent;
            this.transparent =
                this._blendMode === exports.MToonMaterialRenderMode.Transparent ||
                    this._blendMode === exports.MToonMaterialRenderMode.TransparentWithZWrite;
            this._updateShaderCode();
        }
        get debugMode() {
            return this._debugMode;
        }
        set debugMode(m) {
            this._debugMode = m;
            this._updateShaderCode();
        }
        get outlineWidthMode() {
            return this._outlineWidthMode;
        }
        set outlineWidthMode(m) {
            this._outlineWidthMode = m;
            this._updateShaderCode();
        }
        get outlineColorMode() {
            return this._outlineColorMode;
        }
        set outlineColorMode(m) {
            this._outlineColorMode = m;
            this._updateShaderCode();
        }
        get cullMode() {
            return this._cullMode;
        }
        set cullMode(m) {
            this._cullMode = m;
            this._updateCullFace();
        }
        get outlineCullMode() {
            return this._outlineCullMode;
        }
        set outlineCullMode(m) {
            this._outlineCullMode = m;
            this._updateCullFace();
        }
        get zWrite() {
            return this.depthWrite ? 1 : 0;
        }
        set zWrite(i) {
            this.depthWrite = 0.5 <= i;
        }
        get isOutline() {
            return this._isOutline;
        }
        set isOutline(b) {
            this._isOutline = b;
            this._updateShaderCode();
            this._updateCullFace();
        }
        /**
         * Update this material.
         * Usually this will be called via [[VRM.update]] so you don't have to call this manually.
         *
         * @param delta deltaTime since last update
         */
        updateVRMMaterials(delta) {
            this._uvAnimOffsetX = this._uvAnimOffsetX + delta * this.uvAnimScrollX;
            this._uvAnimOffsetY = this._uvAnimOffsetY - delta * this.uvAnimScrollY; // Negative since t axis of uvs are opposite from Unity's one
            this._uvAnimPhase = this._uvAnimPhase + delta * this.uvAnimRotation;
            this._applyUniforms();
        }
        copy(source) {
            super.copy(source);
            // == copy members =========================================================
            this.cutoff = source.cutoff;
            this.color.copy(source.color);
            this.shadeColor.copy(source.shadeColor);
            this.map = source.map;
            this.mainTex_ST.copy(source.mainTex_ST);
            this.shadeTexture = source.shadeTexture;
            this.normalMap = source.normalMap;
            this.normalMapType = source.normalMapType;
            this.normalScale.copy(this.normalScale);
            this.receiveShadowRate = source.receiveShadowRate;
            this.receiveShadowTexture = source.receiveShadowTexture;
            this.shadingGradeRate = source.shadingGradeRate;
            this.shadingGradeTexture = source.shadingGradeTexture;
            this.shadeShift = source.shadeShift;
            this.shadeToony = source.shadeToony;
            this.lightColorAttenuation = source.lightColorAttenuation;
            this.indirectLightIntensity = source.indirectLightIntensity;
            this.rimTexture = source.rimTexture;
            this.rimColor.copy(source.rimColor);
            this.rimLightingMix = source.rimLightingMix;
            this.rimFresnelPower = source.rimFresnelPower;
            this.rimLift = source.rimLift;
            this.sphereAdd = source.sphereAdd;
            this.emissionColor.copy(source.emissionColor);
            this.emissiveMap = source.emissiveMap;
            this.outlineWidthTexture = source.outlineWidthTexture;
            this.outlineWidth = source.outlineWidth;
            this.outlineScaledMaxDistance = source.outlineScaledMaxDistance;
            this.outlineColor.copy(source.outlineColor);
            this.outlineLightingMix = source.outlineLightingMix;
            this.uvAnimMaskTexture = source.uvAnimMaskTexture;
            this.uvAnimScrollX = source.uvAnimScrollX;
            this.uvAnimScrollY = source.uvAnimScrollY;
            this.uvAnimRotation = source.uvAnimRotation;
            this.debugMode = source.debugMode;
            this.blendMode = source.blendMode;
            this.outlineWidthMode = source.outlineWidthMode;
            this.outlineColorMode = source.outlineColorMode;
            this.cullMode = source.cullMode;
            this.outlineCullMode = source.outlineCullMode;
            this.isOutline = source.isOutline;
            return this;
        }
        /**
         * Apply updated uniform variables.
         */
        _applyUniforms() {
            this.uniforms.uvAnimOffsetX.value = this._uvAnimOffsetX;
            this.uniforms.uvAnimOffsetY.value = this._uvAnimOffsetY;
            this.uniforms.uvAnimTheta.value = TAU * this._uvAnimPhase;
            if (!this.shouldApplyUniforms) {
                return;
            }
            this.shouldApplyUniforms = false;
            this.uniforms.cutoff.value = this.cutoff;
            this.uniforms.color.value.setRGB(this.color.x, this.color.y, this.color.z);
            this.uniforms.colorAlpha.value = this.color.w;
            this.uniforms.shadeColor.value.setRGB(this.shadeColor.x, this.shadeColor.y, this.shadeColor.z);
            this.uniforms.map.value = this.map;
            this.uniforms.mainTex_ST.value.copy(this.mainTex_ST);
            this.uniforms.shadeTexture.value = this.shadeTexture;
            this.uniforms.normalMap.value = this.normalMap;
            this.uniforms.normalScale.value.copy(this.normalScale);
            this.uniforms.receiveShadowRate.value = this.receiveShadowRate;
            this.uniforms.receiveShadowTexture.value = this.receiveShadowTexture;
            this.uniforms.shadingGradeRate.value = this.shadingGradeRate;
            this.uniforms.shadingGradeTexture.value = this.shadingGradeTexture;
            this.uniforms.shadeShift.value = this.shadeShift;
            this.uniforms.shadeToony.value = this.shadeToony;
            this.uniforms.lightColorAttenuation.value = this.lightColorAttenuation;
            this.uniforms.indirectLightIntensity.value = this.indirectLightIntensity;
            this.uniforms.rimTexture.value = this.rimTexture;
            this.uniforms.rimColor.value.setRGB(this.rimColor.x, this.rimColor.y, this.rimColor.z);
            this.uniforms.rimLightingMix.value = this.rimLightingMix;
            this.uniforms.rimFresnelPower.value = this.rimFresnelPower;
            this.uniforms.rimLift.value = this.rimLift;
            this.uniforms.sphereAdd.value = this.sphereAdd;
            this.uniforms.emissionColor.value.setRGB(this.emissionColor.x, this.emissionColor.y, this.emissionColor.z);
            this.uniforms.emissiveMap.value = this.emissiveMap;
            this.uniforms.outlineWidthTexture.value = this.outlineWidthTexture;
            this.uniforms.outlineWidth.value = this.outlineWidth;
            this.uniforms.outlineScaledMaxDistance.value = this.outlineScaledMaxDistance;
            this.uniforms.outlineColor.value.setRGB(this.outlineColor.x, this.outlineColor.y, this.outlineColor.z);
            this.uniforms.outlineLightingMix.value = this.outlineLightingMix;
            this.uniforms.uvAnimMaskTexture.value = this.uvAnimMaskTexture;
            // apply color space to uniform colors
            if (this.encoding === THREE.sRGBEncoding) {
                this.uniforms.color.value.convertSRGBToLinear();
                this.uniforms.shadeColor.value.convertSRGBToLinear();
                this.uniforms.rimColor.value.convertSRGBToLinear();
                this.uniforms.emissionColor.value.convertSRGBToLinear();
                this.uniforms.outlineColor.value.convertSRGBToLinear();
            }
            this._updateCullFace();
        }
        _updateShaderCode() {
            this.defines = {
                OUTLINE: this._isOutline,
                BLENDMODE_OPAQUE: this._blendMode === exports.MToonMaterialRenderMode.Opaque,
                BLENDMODE_CUTOUT: this._blendMode === exports.MToonMaterialRenderMode.Cutout,
                BLENDMODE_TRANSPARENT: this._blendMode === exports.MToonMaterialRenderMode.Transparent ||
                    this._blendMode === exports.MToonMaterialRenderMode.TransparentWithZWrite,
                USE_SHADETEXTURE: this.shadeTexture !== null,
                USE_RECEIVESHADOWTEXTURE: this.receiveShadowTexture !== null,
                USE_SHADINGGRADETEXTURE: this.shadingGradeTexture !== null,
                USE_RIMTEXTURE: this.rimTexture !== null,
                USE_SPHEREADD: this.sphereAdd !== null,
                USE_OUTLINEWIDTHTEXTURE: this.outlineWidthTexture !== null,
                USE_UVANIMMASKTEXTURE: this.uvAnimMaskTexture !== null,
                DEBUG_NORMAL: this._debugMode === exports.MToonMaterialDebugMode.Normal,
                DEBUG_LITSHADERATE: this._debugMode === exports.MToonMaterialDebugMode.LitShadeRate,
                DEBUG_UV: this._debugMode === exports.MToonMaterialDebugMode.UV,
                OUTLINE_WIDTH_WORLD: this._outlineWidthMode === exports.MToonMaterialOutlineWidthMode.WorldCoordinates,
                OUTLINE_WIDTH_SCREEN: this._outlineWidthMode === exports.MToonMaterialOutlineWidthMode.ScreenCoordinates,
                OUTLINE_COLOR_FIXED: this._outlineColorMode === exports.MToonMaterialOutlineColorMode.FixedColor,
                OUTLINE_COLOR_MIXED: this._outlineColorMode === exports.MToonMaterialOutlineColorMode.MixedLighting,
            };
            // == texture encodings ====================================================
            const encodings = (this.shadeTexture !== null
                ? getTexelDecodingFunction('shadeTextureTexelToLinear', this.shadeTexture.encoding) + '\n'
                : '') +
                (this.sphereAdd !== null
                    ? getTexelDecodingFunction('sphereAddTexelToLinear', this.sphereAdd.encoding) + '\n'
                    : '') +
                (this.rimTexture !== null
                    ? getTexelDecodingFunction('rimTextureTexelToLinear', this.rimTexture.encoding) + '\n'
                    : '');
            // == generate shader code =================================================
            this.vertexShader = vertexShader;
            this.fragmentShader = encodings + fragmentShader;
            // == set needsUpdate flag =================================================
            this.needsUpdate = true;
        }
        _updateCullFace() {
            if (!this.isOutline) {
                if (this.cullMode === exports.MToonMaterialCullMode.Off) {
                    this.side = THREE.DoubleSide;
                }
                else if (this.cullMode === exports.MToonMaterialCullMode.Front) {
                    this.side = THREE.BackSide;
                }
                else if (this.cullMode === exports.MToonMaterialCullMode.Back) {
                    this.side = THREE.FrontSide;
                }
            }
            else {
                if (this.outlineCullMode === exports.MToonMaterialCullMode.Off) {
                    this.side = THREE.DoubleSide;
                }
                else if (this.outlineCullMode === exports.MToonMaterialCullMode.Front) {
                    this.side = THREE.BackSide;
                }
                else if (this.outlineCullMode === exports.MToonMaterialCullMode.Back) {
                    this.side = THREE.FrontSide;
                }
            }
        }
    }

    var vertexShader$1 = "#include <common>\n\n// #include <uv_pars_vertex>\n#ifdef USE_MAP\n  varying vec2 vUv;\n  uniform vec4 mainTex_ST;\n#endif\n\n#include <uv2_pars_vertex>\n#include <envmap_pars_vertex>\n#include <color_pars_vertex>\n#include <fog_pars_vertex>\n#include <morphtarget_pars_vertex>\n#include <skinning_pars_vertex>\n#include <logdepthbuf_pars_vertex>\n#include <clipping_planes_pars_vertex>\n\nvoid main() {\n\n  // #include <uv_vertex>\n  #ifdef USE_MAP\n    vUv = vec2( mainTex_ST.p * uv.x + mainTex_ST.s, mainTex_ST.q * uv.y + mainTex_ST.t );\n  #endif\n\n  #include <uv2_vertex>\n  #include <color_vertex>\n  #include <skinbase_vertex>\n\n  #ifdef USE_ENVMAP\n\n  #include <beginnormal_vertex>\n  #include <morphnormal_vertex>\n  #include <skinnormal_vertex>\n  #include <defaultnormal_vertex>\n\n  #endif\n\n  #include <begin_vertex>\n  #include <morphtarget_vertex>\n  #include <skinning_vertex>\n  #include <project_vertex>\n  #include <logdepthbuf_vertex>\n\n  #include <worldpos_vertex>\n  #include <clipping_planes_vertex>\n  #include <envmap_vertex>\n  #include <fog_vertex>\n\n}";

    var fragmentShader$1 = "#ifdef RENDERTYPE_CUTOUT\n  uniform float cutoff;\n#endif\n\n#include <common>\n#include <color_pars_fragment>\n#include <uv_pars_fragment>\n#include <uv2_pars_fragment>\n#include <map_pars_fragment>\n// #include <alphamap_pars_fragment>\n// #include <aomap_pars_fragment>\n// #include <lightmap_pars_fragment>\n// #include <envmap_pars_fragment>\n#include <fog_pars_fragment>\n// #include <specularmap_pars_fragment>\n#include <logdepthbuf_pars_fragment>\n#include <clipping_planes_pars_fragment>\n\n// == main procedure ===========================================================\nvoid main() {\n  #include <clipping_planes_fragment>\n\n  vec4 diffuseColor = vec4( 1.0 );\n\n  #include <logdepthbuf_fragment>\n\n  // #include <map_fragment>\n  #ifdef USE_MAP\n    diffuseColor *= mapTexelToLinear( texture2D( map, vUv ) );\n  #endif\n\n  #include <color_fragment>\n  // #include <alphamap_fragment>\n\n  // MToon: alpha\n  // #include <alphatest_fragment>\n  #ifdef RENDERTYPE_CUTOUT\n    if ( diffuseColor.a <= cutoff ) { discard; }\n    diffuseColor.a = 1.0;\n  #endif\n\n  #ifdef RENDERTYPE_OPAQUE\n    diffuseColor.a = 1.0;\n  #endif\n\n  // #include <specularmap_fragment>\n\n  ReflectedLight reflectedLight = ReflectedLight( vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ) );\n\n  // accumulation (baked indirect lighting only)\n  #ifdef USE_LIGHTMAP\n    reflectedLight.indirectDiffuse += texture2D( lightMap, vUv2 ).xyz * lightMapIntensity;\n  #else\n    reflectedLight.indirectDiffuse += vec3( 1.0 );\n  #endif\n\n  // modulation\n  // #include <aomap_fragment>\n\n  reflectedLight.indirectDiffuse *= diffuseColor.rgb;\n  vec3 outgoingLight = reflectedLight.indirectDiffuse;\n\n  // #include <envmap_fragment>\n\n  gl_FragColor = vec4( outgoingLight, diffuseColor.a );\n\n  #include <premultiplied_alpha_fragment>\n  #include <tonemapping_fragment>\n  #include <encodings_fragment>\n  #include <fog_fragment>\n}";

    /* tslint:disable:member-ordering */
    (function (VRMUnlitMaterialRenderType) {
        VRMUnlitMaterialRenderType[VRMUnlitMaterialRenderType["Opaque"] = 0] = "Opaque";
        VRMUnlitMaterialRenderType[VRMUnlitMaterialRenderType["Cutout"] = 1] = "Cutout";
        VRMUnlitMaterialRenderType[VRMUnlitMaterialRenderType["Transparent"] = 2] = "Transparent";
        VRMUnlitMaterialRenderType[VRMUnlitMaterialRenderType["TransparentWithZWrite"] = 3] = "TransparentWithZWrite";
    })(exports.VRMUnlitMaterialRenderType || (exports.VRMUnlitMaterialRenderType = {}));
    /**
     * This is a material that is an equivalent of "VRM/Unlit***" on VRM spec, those materials are already kinda deprecated though...
     */
    class VRMUnlitMaterial extends THREE.ShaderMaterial {
        constructor(parameters) {
            super();
            /**
             * Readonly boolean that indicates this is a [[VRMUnlitMaterial]].
             */
            this.isVRMUnlitMaterial = true;
            this.cutoff = 0.5;
            this.map = null; // _MainTex
            // eslint-disable-next-line @typescript-eslint/naming-convention
            this.mainTex_ST = new THREE.Vector4(0.0, 0.0, 1.0, 1.0); // _MainTex_ST
            this._renderType = exports.VRMUnlitMaterialRenderType.Opaque;
            this.shouldApplyUniforms = true; // when this is true, applyUniforms effects
            if (parameters === undefined) {
                parameters = {};
            }
            // == enabling bunch of stuff ==============================================
            parameters.fog = true;
            parameters.clipping = true;
            parameters.skinning = parameters.skinning || false;
            parameters.morphTargets = parameters.morphTargets || false;
            parameters.morphNormals = parameters.morphNormals || false;
            // == uniforms =============================================================
            parameters.uniforms = THREE.UniformsUtils.merge([
                THREE.UniformsLib.common,
                THREE.UniformsLib.fog,
                {
                    cutoff: { value: 0.5 },
                    // eslint-disable-next-line @typescript-eslint/naming-convention
                    mainTex_ST: { value: new THREE.Vector4(0.0, 0.0, 1.0, 1.0) },
                },
            ]);
            // == finally compile the shader program ===================================
            this.setValues(parameters);
            // == update shader stuff ==================================================
            this._updateShaderCode();
            this._applyUniforms();
        }
        get mainTex() {
            return this.map;
        }
        set mainTex(t) {
            this.map = t;
        }
        get renderType() {
            return this._renderType;
        }
        set renderType(t) {
            this._renderType = t;
            this.depthWrite = this._renderType !== exports.VRMUnlitMaterialRenderType.Transparent;
            this.transparent =
                this._renderType === exports.VRMUnlitMaterialRenderType.Transparent ||
                    this._renderType === exports.VRMUnlitMaterialRenderType.TransparentWithZWrite;
            this._updateShaderCode();
        }
        /**
         * Update this material.
         * Usually this will be called via [[VRM.update]] so you don't have to call this manually.
         *
         * @param delta deltaTime since last update
         */
        updateVRMMaterials(delta) {
            this._applyUniforms();
        }
        copy(source) {
            super.copy(source);
            // == copy members =========================================================
            this.cutoff = source.cutoff;
            this.map = source.map;
            this.mainTex_ST.copy(source.mainTex_ST);
            this.renderType = source.renderType;
            return this;
        }
        /**
         * Apply updated uniform variables.
         */
        _applyUniforms() {
            if (!this.shouldApplyUniforms) {
                return;
            }
            this.shouldApplyUniforms = false;
            this.uniforms.cutoff.value = this.cutoff;
            this.uniforms.map.value = this.map;
            this.uniforms.mainTex_ST.value.copy(this.mainTex_ST);
        }
        _updateShaderCode() {
            this.defines = {
                RENDERTYPE_OPAQUE: this._renderType === exports.VRMUnlitMaterialRenderType.Opaque,
                RENDERTYPE_CUTOUT: this._renderType === exports.VRMUnlitMaterialRenderType.Cutout,
                RENDERTYPE_TRANSPARENT: this._renderType === exports.VRMUnlitMaterialRenderType.Transparent ||
                    this._renderType === exports.VRMUnlitMaterialRenderType.TransparentWithZWrite,
            };
            this.vertexShader = vertexShader$1;
            this.fragmentShader = fragmentShader$1;
            // == set needsUpdate flag =================================================
            this.needsUpdate = true;
        }
    }

    /**
     * An importer that imports VRM materials from a VRM extension of a GLTF.
     */
    class VRMMaterialImporter {
        /**
         * Create a new VRMMaterialImporter.
         *
         * @param options Options of the VRMMaterialImporter
         */
        constructor(options = {}) {
            this._encoding = options.encoding || THREE.LinearEncoding;
            if (this._encoding !== THREE.LinearEncoding && this._encoding !== THREE.sRGBEncoding) {
                console.warn('The specified color encoding might not work properly with VRMMaterialImporter. You might want to use THREE.sRGBEncoding instead.');
            }
            this._requestEnvMap = options.requestEnvMap;
        }
        /**
         * Convert all the materials of given GLTF based on VRM extension field `materialProperties`.
         *
         * @param gltf A parsed result of GLTF taken from GLTFLoader
         */
        convertGLTFMaterials(gltf) {
            var _a;
            return __awaiter(this, void 0, void 0, function* () {
                const vrmExt = (_a = gltf.parser.json.extensions) === null || _a === void 0 ? void 0 : _a.VRM;
                if (!vrmExt) {
                    return null;
                }
                const materialProperties = vrmExt.materialProperties;
                if (!materialProperties) {
                    return null;
                }
                const nodePrimitivesMap = yield gltfExtractPrimitivesFromNodes(gltf);
                const materialList = {};
                const materials = []; // result
                yield Promise.all(Array.from(nodePrimitivesMap.entries()).map(([nodeIndex, primitives]) => __awaiter(this, void 0, void 0, function* () {
                    const schemaNode = gltf.parser.json.nodes[nodeIndex];
                    const schemaMesh = gltf.parser.json.meshes[schemaNode.mesh];
                    yield Promise.all(primitives.map((primitive, primitiveIndex) => __awaiter(this, void 0, void 0, function* () {
                        const schemaPrimitive = schemaMesh.primitives[primitiveIndex];
                        // some glTF might have both `node.mesh` and `node.children` at once
                        // and GLTFLoader handles both mesh primitives and "children" in glTF as "children" in THREE
                        // It seems GLTFLoader handles primitives first then handles "children" in glTF (it's lucky!)
                        // so we should ignore (primitives.length)th and following children of `mesh.children`
                        // TODO: sanitize this after GLTFLoader plugin system gets introduced : https://github.com/mrdoob/three.js/pull/18421
                        if (!schemaPrimitive) {
                            return;
                        }
                        const primitiveGeometry = primitive.geometry;
                        const primitiveVertices = primitiveGeometry.index
                            ? primitiveGeometry.index.count
                            : primitiveGeometry.attributes.position.count / 3;
                        // if primitives material is not an array, make it an array
                        if (!Array.isArray(primitive.material)) {
                            primitive.material = [primitive.material];
                            primitiveGeometry.addGroup(0, primitiveVertices, 0);
                        }
                        // create / push to cache (or pop from cache) vrm materials
                        const vrmMaterialIndex = schemaPrimitive.material;
                        let props = materialProperties[vrmMaterialIndex];
                        if (!props) {
                            console.warn(`VRMMaterialImporter: There are no material definition for material #${vrmMaterialIndex} on VRM extension.`);
                            props = { shader: 'VRM_USE_GLTFSHADER' }; // fallback
                        }
                        let vrmMaterials;
                        if (materialList[vrmMaterialIndex]) {
                            vrmMaterials = materialList[vrmMaterialIndex];
                        }
                        else {
                            vrmMaterials = yield this.createVRMMaterials(primitive.material[0], props, gltf);
                            materialList[vrmMaterialIndex] = vrmMaterials;
                            materials.push(vrmMaterials.surface);
                            if (vrmMaterials.outline) {
                                materials.push(vrmMaterials.outline);
                            }
                        }
                        // surface
                        primitive.material[0] = vrmMaterials.surface;
                        // envmap
                        if (this._requestEnvMap && vrmMaterials.surface.isMeshStandardMaterial) {
                            this._requestEnvMap().then((envMap) => {
                                vrmMaterials.surface.envMap = envMap;
                                vrmMaterials.surface.needsUpdate = true;
                            });
                        }
                        // render order
                        primitive.renderOrder = props.renderQueue || 2000;
                        // outline ("2 pass shading using groups" trick here)
                        if (vrmMaterials.outline) {
                            primitive.material[1] = vrmMaterials.outline;
                            primitiveGeometry.addGroup(0, primitiveVertices, 1);
                        }
                    })));
                })));
                return materials;
            });
        }
        createVRMMaterials(originalMaterial, vrmProps, gltf) {
            return __awaiter(this, void 0, void 0, function* () {
                let newSurface;
                let newOutline;
                if (vrmProps.shader === 'VRM/MToon') {
                    const params = yield this._extractMaterialProperties(originalMaterial, vrmProps, gltf);
                    // we need to get rid of these properties
                    ['srcBlend', 'dstBlend', 'isFirstSetup'].forEach((name) => {
                        if (params[name] !== undefined) {
                            delete params[name];
                        }
                    });
                    // these textures must be sRGB Encoding, depends on current colorspace
                    ['mainTex', 'shadeTexture', 'emissionMap', 'sphereAdd', 'rimTexture'].forEach((name) => {
                        if (params[name] !== undefined) {
                            params[name].encoding = this._encoding;
                        }
                    });
                    // specify uniform color encodings
                    params.encoding = this._encoding;
                    // done
                    newSurface = new MToonMaterial(params);
                    // outline
                    if (params.outlineWidthMode !== exports.MToonMaterialOutlineWidthMode.None) {
                        params.isOutline = true;
                        newOutline = new MToonMaterial(params);
                    }
                }
                else if (vrmProps.shader === 'VRM/UnlitTexture') {
                    // this is very legacy
                    const params = yield this._extractMaterialProperties(originalMaterial, vrmProps, gltf);
                    params.renderType = exports.VRMUnlitMaterialRenderType.Opaque;
                    newSurface = new VRMUnlitMaterial(params);
                }
                else if (vrmProps.shader === 'VRM/UnlitCutout') {
                    // this is very legacy
                    const params = yield this._extractMaterialProperties(originalMaterial, vrmProps, gltf);
                    params.renderType = exports.VRMUnlitMaterialRenderType.Cutout;
                    newSurface = new VRMUnlitMaterial(params);
                }
                else if (vrmProps.shader === 'VRM/UnlitTransparent') {
                    // this is very legacy
                    const params = yield this._extractMaterialProperties(originalMaterial, vrmProps, gltf);
                    params.renderType = exports.VRMUnlitMaterialRenderType.Transparent;
                    newSurface = new VRMUnlitMaterial(params);
                }
                else if (vrmProps.shader === 'VRM/UnlitTransparentZWrite') {
                    // this is very legacy
                    const params = yield this._extractMaterialProperties(originalMaterial, vrmProps, gltf);
                    params.renderType = exports.VRMUnlitMaterialRenderType.TransparentWithZWrite;
                    newSurface = new VRMUnlitMaterial(params);
                }
                else {
                    if (vrmProps.shader !== 'VRM_USE_GLTFSHADER') {
                        console.warn(`Unknown shader detected: "${vrmProps.shader}"`);
                        // then presume as VRM_USE_GLTFSHADER
                    }
                    newSurface = this._convertGLTFMaterial(originalMaterial.clone());
                }
                newSurface.name = originalMaterial.name;
                newSurface.userData = JSON.parse(JSON.stringify(originalMaterial.userData));
                newSurface.userData.vrmMaterialProperties = vrmProps;
                if (newOutline) {
                    newOutline.name = originalMaterial.name + ' (Outline)';
                    newOutline.userData = JSON.parse(JSON.stringify(originalMaterial.userData));
                    newOutline.userData.vrmMaterialProperties = vrmProps;
                }
                return {
                    surface: newSurface,
                    outline: newOutline,
                };
            });
        }
        _renameMaterialProperty(name) {
            if (name[0] !== '_') {
                console.warn(`VRMMaterials: Given property name "${name}" might be invalid`);
                return name;
            }
            name = name.substring(1);
            if (!/[A-Z]/.test(name[0])) {
                console.warn(`VRMMaterials: Given property name "${name}" might be invalid`);
                return name;
            }
            return name[0].toLowerCase() + name.substring(1);
        }
        _convertGLTFMaterial(material) {
            if (material.isMeshStandardMaterial) {
                const mtl = material;
                if (mtl.map) {
                    mtl.map.encoding = this._encoding;
                }
                if (mtl.emissiveMap) {
                    mtl.emissiveMap.encoding = this._encoding;
                }
                if (this._encoding === THREE.LinearEncoding) {
                    mtl.color.convertLinearToSRGB();
                    mtl.emissive.convertLinearToSRGB();
                }
            }
            if (material.isMeshBasicMaterial) {
                const mtl = material;
                if (mtl.map) {
                    mtl.map.encoding = this._encoding;
                }
                if (this._encoding === THREE.LinearEncoding) {
                    mtl.color.convertLinearToSRGB();
                }
            }
            return material;
        }
        _extractMaterialProperties(originalMaterial, vrmProps, gltf) {
            const taskList = [];
            const params = {};
            // extract texture properties
            if (vrmProps.textureProperties) {
                for (const name of Object.keys(vrmProps.textureProperties)) {
                    const newName = this._renameMaterialProperty(name);
                    const textureIndex = vrmProps.textureProperties[name];
                    taskList.push(gltf.parser.getDependency('texture', textureIndex).then((texture) => {
                        params[newName] = texture;
                    }));
                }
            }
            // extract float properties
            if (vrmProps.floatProperties) {
                for (const name of Object.keys(vrmProps.floatProperties)) {
                    const newName = this._renameMaterialProperty(name);
                    params[newName] = vrmProps.floatProperties[name];
                }
            }
            // extract vector (color tbh) properties
            if (vrmProps.vectorProperties) {
                for (const name of Object.keys(vrmProps.vectorProperties)) {
                    let newName = this._renameMaterialProperty(name);
                    // if this is textureST (same name as texture name itself), add '_ST'
                    const isTextureST = [
                        '_MainTex',
                        '_ShadeTexture',
                        '_BumpMap',
                        '_ReceiveShadowTexture',
                        '_ShadingGradeTexture',
                        '_RimTexture',
                        '_SphereAdd',
                        '_EmissionMap',
                        '_OutlineWidthTexture',
                        '_UvAnimMaskTexture',
                    ].some((textureName) => name === textureName);
                    if (isTextureST) {
                        newName += '_ST';
                    }
                    params[newName] = new THREE.Vector4(...vrmProps.vectorProperties[name]);
                }
            }
            // set whether it needs skinning and morphing or not
            params.skinning = originalMaterial.skinning || false;
            params.morphTargets = originalMaterial.morphTargets || false;
            params.morphNormals = originalMaterial.morphNormals || false;
            return Promise.all(taskList).then(() => params);
        }
    }

    /**
     * An importer that imports a {@link VRMMeta} from a VRM extension of a GLTF.
     */
    class VRMMetaImporter {
        constructor(options) {
            var _a;
            this.ignoreTexture = (_a = options === null || options === void 0 ? void 0 : options.ignoreTexture) !== null && _a !== void 0 ? _a : false;
        }
        import(gltf) {
            var _a;
            return __awaiter(this, void 0, void 0, function* () {
                const vrmExt = (_a = gltf.parser.json.extensions) === null || _a === void 0 ? void 0 : _a.VRM;
                if (!vrmExt) {
                    return null;
                }
                const schemaMeta = vrmExt.meta;
                if (!schemaMeta) {
                    return null;
                }
                let texture;
                if (!this.ignoreTexture && schemaMeta.texture != null && schemaMeta.texture !== -1) {
                    texture = yield gltf.parser.getDependency('texture', schemaMeta.texture);
                }
                return {
                    allowedUserName: schemaMeta.allowedUserName,
                    author: schemaMeta.author,
                    commercialUssageName: schemaMeta.commercialUssageName,
                    contactInformation: schemaMeta.contactInformation,
                    licenseName: schemaMeta.licenseName,
                    otherLicenseUrl: schemaMeta.otherLicenseUrl,
                    otherPermissionUrl: schemaMeta.otherPermissionUrl,
                    reference: schemaMeta.reference,
                    sexualUssageName: schemaMeta.sexualUssageName,
                    texture: texture !== null && texture !== void 0 ? texture : undefined,
                    title: schemaMeta.title,
                    version: schemaMeta.version,
                    violentUssageName: schemaMeta.violentUssageName,
                };
            });
        }
    }

    const _matA = new THREE.Matrix4();
    /**
     * A compat function for `Matrix4.invert()` / `Matrix4.getInverse()`.
     * `Matrix4.invert()` is introduced in r123 and `Matrix4.getInverse()` emits a warning.
     * We are going to use this compat for a while.
     * @param target A target matrix
     */
    function mat4InvertCompat(target) {
        if (target.invert) {
            target.invert();
        }
        else {
            target.getInverse(_matA.copy(target));
        }
        return target;
    }

    class Matrix4InverseCache {
        constructor(matrix) {
            /**
             * A cache of inverse of current matrix.
             */
            this._inverseCache = new THREE.Matrix4();
            /**
             * A flag that makes it want to recalculate its {@link _inverseCache}.
             * Will be set `true` when `elements` are mutated and be used in `getInverse`.
             */
            this._shouldUpdateInverse = true;
            this.matrix = matrix;
            const handler = {
                set: (obj, prop, newVal) => {
                    this._shouldUpdateInverse = true;
                    obj[prop] = newVal;
                    return true;
                },
            };
            this._originalElements = matrix.elements;
            matrix.elements = new Proxy(matrix.elements, handler);
        }
        /**
         * Inverse of given matrix.
         * Note that it will return its internal private instance.
         * Make sure copying this before mutate this.
         */
        get inverse() {
            if (this._shouldUpdateInverse) {
                mat4InvertCompat(this._inverseCache.copy(this.matrix));
                this._shouldUpdateInverse = false;
            }
            return this._inverseCache;
        }
        revert() {
            this.matrix.elements = this._originalElements;
        }
    }

    // based on
    // http://rocketjump.skr.jp/unity3d/109/
    // https://github.com/dwango/UniVRM/blob/master/Scripts/SpringBone/VRMSpringBone.cs
    const IDENTITY_MATRIX4 = Object.freeze(new THREE.Matrix4());
    const IDENTITY_QUATERNION = Object.freeze(new THREE.Quaternion());
    // 計算中の一時保存用変数（一度インスタンスを作ったらあとは使い回す）
    const _v3A$2 = new THREE.Vector3();
    const _v3B$1 = new THREE.Vector3();
    const _v3C$1 = new THREE.Vector3();
    const _quatA$1 = new THREE.Quaternion();
    const _matA$1 = new THREE.Matrix4();
    const _matB = new THREE.Matrix4();
    /**
     * A class represents a single spring bone of a VRM.
     * It should be managed by a [[VRMSpringBoneManager]].
     */
    class VRMSpringBone {
        /**
         * Create a new VRMSpringBone.
         *
         * @param bone An Object3D that will be attached to this bone
         * @param params Several parameters related to behavior of the spring bone
         */
        constructor(bone, params = {}) {
            var _a, _b, _c, _d, _e, _f;
            /**
             * Current position of child tail, in world unit. Will be used for verlet integration.
             */
            this._currentTail = new THREE.Vector3();
            /**
             * Previous position of child tail, in world unit. Will be used for verlet integration.
             */
            this._prevTail = new THREE.Vector3();
            /**
             * Next position of child tail, in world unit. Will be used for verlet integration.
             * Actually used only in [[update]] and it's kind of temporary variable.
             */
            this._nextTail = new THREE.Vector3();
            /**
             * Initial axis of the bone, in local unit.
             */
            this._boneAxis = new THREE.Vector3();
            /**
             * Position of this bone in relative space, kind of a temporary variable.
             */
            this._centerSpacePosition = new THREE.Vector3();
            /**
             * This springbone will be calculated based on the space relative from this object.
             * If this is `null`, springbone will be calculated in world space.
             */
            this._center = null;
            /**
             * Rotation of parent bone, in world unit.
             * We should update this constantly in [[update]].
             */
            this._parentWorldRotation = new THREE.Quaternion();
            /**
             * Initial state of the local matrix of the bone.
             */
            this._initialLocalMatrix = new THREE.Matrix4();
            /**
             * Initial state of the rotation of the bone.
             */
            this._initialLocalRotation = new THREE.Quaternion();
            /**
             * Initial state of the position of its child.
             */
            this._initialLocalChildPosition = new THREE.Vector3();
            this.bone = bone; // uniVRMでの parent
            this.bone.matrixAutoUpdate = false; // updateにより計算されるのでthree.js内での自動処理は不要
            this.radius = (_a = params.radius) !== null && _a !== void 0 ? _a : 0.02;
            this.stiffnessForce = (_b = params.stiffnessForce) !== null && _b !== void 0 ? _b : 1.0;
            this.gravityDir = params.gravityDir
                ? new THREE.Vector3().copy(params.gravityDir)
                : new THREE.Vector3().set(0.0, -1.0, 0.0);
            this.gravityPower = (_c = params.gravityPower) !== null && _c !== void 0 ? _c : 0.0;
            this.dragForce = (_d = params.dragForce) !== null && _d !== void 0 ? _d : 0.4;
            this.colliders = (_e = params.colliders) !== null && _e !== void 0 ? _e : [];
            this._centerSpacePosition.setFromMatrixPosition(this.bone.matrixWorld);
            this._initialLocalMatrix.copy(this.bone.matrix);
            this._initialLocalRotation.copy(this.bone.quaternion);
            if (this.bone.children.length === 0) {
                // 末端のボーン。子ボーンがいないため「自分の少し先」が子ボーンということにする
                // https://github.com/dwango/UniVRM/blob/master/Assets/VRM/UniVRM/Scripts/SpringBone/VRMSpringBone.cs#L246
                this._initialLocalChildPosition.copy(this.bone.position).normalize().multiplyScalar(0.07); // magic number! derives from original source
            }
            else {
                const firstChild = this.bone.children[0];
                this._initialLocalChildPosition.copy(firstChild.position);
            }
            this.bone.localToWorld(this._currentTail.copy(this._initialLocalChildPosition));
            this._prevTail.copy(this._currentTail);
            this._nextTail.copy(this._currentTail);
            this._boneAxis.copy(this._initialLocalChildPosition).normalize();
            this._centerSpaceBoneLength = _v3A$2
                .copy(this._initialLocalChildPosition)
                .applyMatrix4(this.bone.matrixWorld)
                .sub(this._centerSpacePosition)
                .length();
            this.center = (_f = params.center) !== null && _f !== void 0 ? _f : null;
        }
        get center() {
            return this._center;
        }
        set center(center) {
            var _a;
            // convert tails to world space
            this._getMatrixCenterToWorld(_matA$1);
            this._currentTail.applyMatrix4(_matA$1);
            this._prevTail.applyMatrix4(_matA$1);
            this._nextTail.applyMatrix4(_matA$1);
            // uninstall inverse cache
            if ((_a = this._center) === null || _a === void 0 ? void 0 : _a.userData.inverseCacheProxy) {
                this._center.userData.inverseCacheProxy.revert();
                delete this._center.userData.inverseCacheProxy;
            }
            // change the center
            this._center = center;
            // install inverse cache
            if (this._center) {
                if (!this._center.userData.inverseCacheProxy) {
                    this._center.userData.inverseCacheProxy = new Matrix4InverseCache(this._center.matrixWorld);
                }
            }
            // convert tails to center space
            this._getMatrixWorldToCenter(_matA$1);
            this._currentTail.applyMatrix4(_matA$1);
            this._prevTail.applyMatrix4(_matA$1);
            this._nextTail.applyMatrix4(_matA$1);
            // convert center space dependant state
            _matA$1.multiply(this.bone.matrixWorld); // 🔥 ??
            this._centerSpacePosition.setFromMatrixPosition(_matA$1);
            this._centerSpaceBoneLength = _v3A$2
                .copy(this._initialLocalChildPosition)
                .applyMatrix4(_matA$1)
                .sub(this._centerSpacePosition)
                .length();
        }
        /**
         * Reset the state of this bone.
         * You might want to call [[VRMSpringBoneManager.reset]] instead.
         */
        reset() {
            this.bone.quaternion.copy(this._initialLocalRotation);
            // We need to update its matrixWorld manually, since we tweaked the bone by our hand
            this.bone.updateMatrix();
            this.bone.matrixWorld.multiplyMatrices(this._getParentMatrixWorld(), this.bone.matrix);
            this._centerSpacePosition.setFromMatrixPosition(this.bone.matrixWorld);
            // Apply updated position to tail states
            this.bone.localToWorld(this._currentTail.copy(this._initialLocalChildPosition));
            this._prevTail.copy(this._currentTail);
            this._nextTail.copy(this._currentTail);
        }
        /**
         * Update the state of this bone.
         * You might want to call [[VRMSpringBoneManager.update]] instead.
         *
         * @param delta deltaTime
         */
        update(delta) {
            if (delta <= 0)
                return;
            // 親スプリングボーンの姿勢は常に変化している。
            // それに基づいて処理直前に自分のworldMatrixを更新しておく
            this.bone.matrixWorld.multiplyMatrices(this._getParentMatrixWorld(), this.bone.matrix);
            if (this.bone.parent) {
                // SpringBoneは親から順に処理されていくため、
                // 親のmatrixWorldは最新状態の前提でworldMatrixからquaternionを取り出す。
                // 制限はあるけれど、計算は少ないのでgetWorldQuaternionではなくこの方法を取る。
                getWorldQuaternionLite(this.bone.parent, this._parentWorldRotation);
            }
            else {
                this._parentWorldRotation.copy(IDENTITY_QUATERNION);
            }
            // Get bone position in center space
            this._getMatrixWorldToCenter(_matA$1);
            _matA$1.multiply(this.bone.matrixWorld); // 🔥 ??
            this._centerSpacePosition.setFromMatrixPosition(_matA$1);
            // Get parent position in center space
            this._getMatrixWorldToCenter(_matB);
            _matB.multiply(this._getParentMatrixWorld());
            // several parameters
            const stiffness = this.stiffnessForce * delta;
            const external = _v3B$1.copy(this.gravityDir).multiplyScalar(this.gravityPower * delta);
            // verlet積分で次の位置を計算
            this._nextTail
                .copy(this._currentTail)
                .add(_v3A$2
                .copy(this._currentTail)
                .sub(this._prevTail)
                .multiplyScalar(1 - this.dragForce)) // 前フレームの移動を継続する(減衰もあるよ)
                .add(_v3A$2
                .copy(this._boneAxis)
                .applyMatrix4(this._initialLocalMatrix)
                .applyMatrix4(_matB)
                .sub(this._centerSpacePosition)
                .normalize()
                .multiplyScalar(stiffness)) // 親の回転による子ボーンの移動目標
                .add(external); // 外力による移動量
            // normalize bone length
            this._nextTail
                .sub(this._centerSpacePosition)
                .normalize()
                .multiplyScalar(this._centerSpaceBoneLength)
                .add(this._centerSpacePosition);
            // Collisionで移動
            this._collision(this._nextTail);
            this._prevTail.copy(this._currentTail);
            this._currentTail.copy(this._nextTail);
            // Apply rotation, convert vector3 thing into actual quaternion
            // Original UniVRM is doing world unit calculus at here but we're gonna do this on local unit
            // since Three.js is not good at world coordination stuff
            const initialCenterSpaceMatrixInv = mat4InvertCompat(_matA$1.copy(_matB.multiply(this._initialLocalMatrix)));
            const applyRotation = _quatA$1.setFromUnitVectors(this._boneAxis, _v3A$2.copy(this._nextTail).applyMatrix4(initialCenterSpaceMatrixInv).normalize());
            this.bone.quaternion.copy(this._initialLocalRotation).multiply(applyRotation);
            // We need to update its matrixWorld manually, since we tweaked the bone by our hand
            this.bone.updateMatrix();
            this.bone.matrixWorld.multiplyMatrices(this._getParentMatrixWorld(), this.bone.matrix);
        }
        /**
         * Do collision math against every colliders attached to this bone.
         *
         * @param tail The tail you want to process
         */
        _collision(tail) {
            this.colliders.forEach((collider) => {
                this._getMatrixWorldToCenter(_matA$1);
                _matA$1.multiply(collider.matrixWorld);
                const colliderCenterSpacePosition = _v3A$2.setFromMatrixPosition(_matA$1);
                const colliderRadius = collider.geometry.boundingSphere.radius; // the bounding sphere is guaranteed to be exist by VRMSpringBoneImporter._createColliderMesh
                const r = this.radius + colliderRadius;
                if (tail.distanceToSquared(colliderCenterSpacePosition) <= r * r) {
                    // ヒット。Colliderの半径方向に押し出す
                    const normal = _v3B$1.subVectors(tail, colliderCenterSpacePosition).normalize();
                    const posFromCollider = _v3C$1.addVectors(colliderCenterSpacePosition, normal.multiplyScalar(r));
                    // normalize bone length
                    tail.copy(posFromCollider
                        .sub(this._centerSpacePosition)
                        .normalize()
                        .multiplyScalar(this._centerSpaceBoneLength)
                        .add(this._centerSpacePosition));
                }
            });
        }
        /**
         * Create a matrix that converts center space into world space.
         * @param target Target matrix
         */
        _getMatrixCenterToWorld(target) {
            if (this._center) {
                target.copy(this._center.matrixWorld);
            }
            else {
                target.identity();
            }
            return target;
        }
        /**
         * Create a matrix that converts world space into center space.
         * @param target Target matrix
         */
        _getMatrixWorldToCenter(target) {
            if (this._center) {
                target.copy(this._center.userData.inverseCacheProxy.inverse);
            }
            else {
                target.identity();
            }
            return target;
        }
        /**
         * Returns the world matrix of its parent object.
         */
        _getParentMatrixWorld() {
            return this.bone.parent ? this.bone.parent.matrixWorld : IDENTITY_MATRIX4;
        }
    }

    /**
     * A class manages every spring bones on a VRM.
     */
    class VRMSpringBoneManager {
        /**
         * Create a new [[VRMSpringBoneManager]]
         *
         * @param springBoneGroupList An array of [[VRMSpringBoneGroup]]
         */
        constructor(colliderGroups, springBoneGroupList) {
            this.colliderGroups = [];
            this.springBoneGroupList = [];
            this.colliderGroups = colliderGroups;
            this.springBoneGroupList = springBoneGroupList;
        }
        /**
         * Set all bones be calculated based on the space relative from this object.
         * If `null` is given, springbone will be calculated in world space.
         * @param root Root object, or `null`
         */
        setCenter(root) {
            this.springBoneGroupList.forEach((springBoneGroup) => {
                springBoneGroup.forEach((springBone) => {
                    springBone.center = root;
                });
            });
        }
        /**
         * Update every spring bone attached to this manager.
         *
         * @param delta deltaTime
         */
        lateUpdate(delta) {
            this.springBoneGroupList.forEach((springBoneGroup) => {
                springBoneGroup.forEach((springBone) => {
                    springBone.update(delta);
                });
            });
        }
        /**
         * Reset every spring bone attached to this manager.
         */
        reset() {
            this.springBoneGroupList.forEach((springBoneGroup) => {
                springBoneGroup.forEach((springBone) => {
                    springBone.reset();
                });
            });
        }
    }

    const _v3A$3 = new THREE.Vector3();
    const _colliderMaterial = new THREE.MeshBasicMaterial({ visible: false });
    /**
     * An importer that imports a [[VRMSpringBoneManager]] from a VRM extension of a GLTF.
     */
    class VRMSpringBoneImporter {
        /**
         * Import a [[VRMLookAtHead]] from a VRM.
         *
         * @param gltf A parsed result of GLTF taken from GLTFLoader
         */
        import(gltf) {
            var _a;
            return __awaiter(this, void 0, void 0, function* () {
                const vrmExt = (_a = gltf.parser.json.extensions) === null || _a === void 0 ? void 0 : _a.VRM;
                if (!vrmExt)
                    return null;
                const schemaSecondaryAnimation = vrmExt.secondaryAnimation;
                if (!schemaSecondaryAnimation)
                    return null;
                // 衝突判定球体メッシュ。
                const colliderGroups = yield this._importColliderMeshGroups(gltf, schemaSecondaryAnimation);
                // 同じ属性（stiffinessやdragForceが同じ）のボーンはboneGroupにまとめられている。
                // 一列だけではないことに注意。
                const springBoneGroupList = yield this._importSpringBoneGroupList(gltf, schemaSecondaryAnimation, colliderGroups);
                return new VRMSpringBoneManager(colliderGroups, springBoneGroupList);
            });
        }
        _createSpringBone(bone, params = {}) {
            return new VRMSpringBone(bone, params);
        }
        _importSpringBoneGroupList(gltf, schemaSecondaryAnimation, colliderGroups) {
            return __awaiter(this, void 0, void 0, function* () {
                const springBoneGroups = schemaSecondaryAnimation.boneGroups || [];
                const springBoneGroupList = [];
                yield Promise.all(springBoneGroups.map((vrmBoneGroup) => __awaiter(this, void 0, void 0, function* () {
                    if (vrmBoneGroup.stiffiness === undefined ||
                        vrmBoneGroup.gravityDir === undefined ||
                        vrmBoneGroup.gravityDir.x === undefined ||
                        vrmBoneGroup.gravityDir.y === undefined ||
                        vrmBoneGroup.gravityDir.z === undefined ||
                        vrmBoneGroup.gravityPower === undefined ||
                        vrmBoneGroup.dragForce === undefined ||
                        vrmBoneGroup.hitRadius === undefined ||
                        vrmBoneGroup.colliderGroups === undefined ||
                        vrmBoneGroup.bones === undefined ||
                        vrmBoneGroup.center === undefined) {
                        return;
                    }
                    const stiffnessForce = vrmBoneGroup.stiffiness;
                    const gravityDir = new THREE.Vector3(vrmBoneGroup.gravityDir.x, vrmBoneGroup.gravityDir.y, -vrmBoneGroup.gravityDir.z);
                    const gravityPower = vrmBoneGroup.gravityPower;
                    const dragForce = vrmBoneGroup.dragForce;
                    const radius = vrmBoneGroup.hitRadius;
                    const colliders = [];
                    vrmBoneGroup.colliderGroups.forEach((colliderIndex) => {
                        colliders.push(...colliderGroups[colliderIndex].colliders);
                    });
                    const springBoneGroup = [];
                    yield Promise.all(vrmBoneGroup.bones.map((nodeIndex) => __awaiter(this, void 0, void 0, function* () {
                        // VRMの情報から「揺れモノ」ボーンのルートが取れる
                        const springRootBone = yield gltf.parser.getDependency('node', nodeIndex);
                        const center = vrmBoneGroup.center !== -1 ? yield gltf.parser.getDependency('node', vrmBoneGroup.center) : null;
                        // it's weird but there might be cases we can't find the root bone
                        if (!springRootBone) {
                            return;
                        }
                        springRootBone.traverse((bone) => {
                            const springBone = this._createSpringBone(bone, {
                                radius,
                                stiffnessForce,
                                gravityDir,
                                gravityPower,
                                dragForce,
                                colliders,
                                center,
                            });
                            springBoneGroup.push(springBone);
                        });
                    })));
                    springBoneGroupList.push(springBoneGroup);
                })));
                return springBoneGroupList;
            });
        }
        /**
         * Create an array of [[VRMSpringBoneColliderGroup]].
         *
         * @param gltf A parsed result of GLTF taken from GLTFLoader
         * @param schemaSecondaryAnimation A `secondaryAnimation` field of VRM
         */
        _importColliderMeshGroups(gltf, schemaSecondaryAnimation) {
            return __awaiter(this, void 0, void 0, function* () {
                const vrmColliderGroups = schemaSecondaryAnimation.colliderGroups;
                if (vrmColliderGroups === undefined)
                    return [];
                const colliderGroups = [];
                vrmColliderGroups.forEach((colliderGroup) => __awaiter(this, void 0, void 0, function* () {
                    if (colliderGroup.node === undefined || colliderGroup.colliders === undefined) {
                        return;
                    }
                    const bone = yield gltf.parser.getDependency('node', colliderGroup.node);
                    const colliders = [];
                    colliderGroup.colliders.forEach((collider) => {
                        if (collider.offset === undefined ||
                            collider.offset.x === undefined ||
                            collider.offset.y === undefined ||
                            collider.offset.z === undefined ||
                            collider.radius === undefined) {
                            return;
                        }
                        const offset = _v3A$3.set(collider.offset.x, collider.offset.y, -collider.offset.z);
                        const colliderMesh = this._createColliderMesh(collider.radius, offset);
                        bone.add(colliderMesh);
                        colliders.push(colliderMesh);
                    });
                    const colliderMeshGroup = {
                        node: colliderGroup.node,
                        colliders,
                    };
                    colliderGroups.push(colliderMeshGroup);
                }));
                return colliderGroups;
            });
        }
        /**
         * Create a collider mesh.
         *
         * @param radius Radius of the new collider mesh
         * @param offset Offest of the new collider mesh
         */
        _createColliderMesh(radius, offset) {
            const colliderMesh = new THREE.Mesh(new THREE.SphereBufferGeometry(radius, 8, 4), _colliderMaterial);
            colliderMesh.position.copy(offset);
            // the name have to be this in order to exclude colliders from bounding box
            // (See Viewer.ts, search for child.name === 'vrmColliderSphere')
            colliderMesh.name = 'vrmColliderSphere';
            // We will use the radius of the sphere for collision vs bones.
            // `boundingSphere` must be created to compute the radius.
            colliderMesh.geometry.computeBoundingSphere();
            return colliderMesh;
        }
    }

    /**
     * An importer that imports a [[VRM]] from a VRM extension of a GLTF.
     */
    class VRMImporter {
        /**
         * Create a new VRMImporter.
         *
         * @param options [[VRMImporterOptions]], optionally contains importers for each component
         */
        constructor(options = {}) {
            this._metaImporter = options.metaImporter || new VRMMetaImporter();
            this._blendShapeImporter = options.blendShapeImporter || new VRMBlendShapeImporter();
            this._lookAtImporter = options.lookAtImporter || new VRMLookAtImporter();
            this._humanoidImporter = options.humanoidImporter || new VRMHumanoidImporter();
            this._firstPersonImporter = options.firstPersonImporter || new VRMFirstPersonImporter();
            this._materialImporter = options.materialImporter || new VRMMaterialImporter();
            this._springBoneImporter = options.springBoneImporter || new VRMSpringBoneImporter();
        }
        /**
         * Receive a GLTF object retrieved from `THREE.GLTFLoader` and create a new [[VRM]] instance.
         *
         * @param gltf A parsed result of GLTF taken from GLTFLoader
         */
        import(gltf) {
            return __awaiter(this, void 0, void 0, function* () {
                if (gltf.parser.json.extensions === undefined || gltf.parser.json.extensions.VRM === undefined) {
                    throw new Error('Could not find VRM extension on the GLTF');
                }
                const scene = gltf.scene;
                scene.updateMatrixWorld(false);
                // Skinned object should not be frustumCulled
                // Since pre-skinned position might be outside of view
                scene.traverse((object3d) => {
                    if (object3d.isMesh) {
                        object3d.frustumCulled = false;
                    }
                });
                const meta = (yield this._metaImporter.import(gltf)) || undefined;
                const materials = (yield this._materialImporter.convertGLTFMaterials(gltf)) || undefined;
                const humanoid = (yield this._humanoidImporter.import(gltf)) || undefined;
                const firstPerson = humanoid ? (yield this._firstPersonImporter.import(gltf, humanoid)) || undefined : undefined;
                const blendShapeProxy = (yield this._blendShapeImporter.import(gltf)) || undefined;
                const lookAt = firstPerson && blendShapeProxy && humanoid
                    ? (yield this._lookAtImporter.import(gltf, firstPerson, blendShapeProxy, humanoid)) || undefined
                    : undefined;
                const springBoneManager = (yield this._springBoneImporter.import(gltf)) || undefined;
                return new VRM({
                    scene: gltf.scene,
                    meta,
                    materials,
                    humanoid,
                    firstPerson,
                    blendShapeProxy,
                    lookAt,
                    springBoneManager,
                });
            });
        }
    }

    /**
     * A class that represents a single VRM model.
     * See the documentation of [[VRM.from]] for the most basic use of VRM.
     */
    class VRM {
        /**
         * Create a new VRM instance.
         *
         * @param params [[VRMParameters]] that represents components of the VRM
         */
        constructor(params) {
            this.scene = params.scene;
            this.humanoid = params.humanoid;
            this.blendShapeProxy = params.blendShapeProxy;
            this.firstPerson = params.firstPerson;
            this.lookAt = params.lookAt;
            this.materials = params.materials;
            this.springBoneManager = params.springBoneManager;
            this.meta = params.meta;
        }
        /**
         * Create a new VRM from a parsed result of GLTF taken from GLTFLoader.
         * It's probably a thing what you want to get started with VRMs.
         *
         * @example Most basic use of VRM
         * ```
         * const scene = new THREE.Scene();
         *
         * new THREE.GLTFLoader().load( 'models/three-vrm-girl.vrm', ( gltf ) => {
         *
         *   THREE.VRM.from( gltf ).then( ( vrm ) => {
         *
         *     scene.add( vrm.scene );
         *
         *   } );
         *
         * } );
         * ```
         *
         * @param gltf A parsed GLTF object taken from GLTFLoader
         * @param options Options that will be used in importer
         */
        static from(gltf, options = {}) {
            return __awaiter(this, void 0, void 0, function* () {
                const importer = new VRMImporter(options);
                return yield importer.import(gltf);
            });
        }
        /**
         * **You need to call this on your update loop.**
         *
         * This function updates every VRM components.
         *
         * @param delta deltaTime
         */
        update(delta) {
            if (this.lookAt) {
                this.lookAt.update(delta);
            }
            if (this.blendShapeProxy) {
                this.blendShapeProxy.update();
            }
            if (this.springBoneManager) {
                this.springBoneManager.lateUpdate(delta);
            }
            if (this.materials) {
                this.materials.forEach((material) => {
                    if (material.updateVRMMaterials) {
                        material.updateVRMMaterials(delta);
                    }
                });
            }
        }
        /**
         * Dispose everything about the VRM instance.
         */
        dispose() {
            var _a, _b;
            const scene = this.scene;
            if (scene) {
                deepDispose(scene);
            }
            (_b = (_a = this.meta) === null || _a === void 0 ? void 0 : _a.texture) === null || _b === void 0 ? void 0 : _b.dispose();
        }
    }

    const _v2A = new THREE.Vector2();
    const _camera = new THREE.OrthographicCamera(-1, 1, -1, 1, -1, 1);
    const _material = new THREE.MeshBasicMaterial({ color: 0xffffff, side: THREE.DoubleSide });
    const _plane = new THREE.Mesh(new THREE.PlaneBufferGeometry(2, 2), _material);
    const _scene = new THREE.Scene();
    _scene.add(_plane);
    /**
     * Extract a thumbnail image blob from a {@link VRM}.
     * If the vrm does not have a thumbnail, it will throw an error.
     * @param renderer Renderer
     * @param vrm VRM with a thumbnail
     * @param size width / height of the image
     */
    function extractThumbnailBlob(renderer, vrm, size = 512) {
        var _a;
        // get the texture
        const texture = (_a = vrm.meta) === null || _a === void 0 ? void 0 : _a.texture;
        if (!texture) {
            throw new Error('extractThumbnailBlob: This VRM does not have a thumbnail');
        }
        const canvas = renderer.getContext().canvas;
        // store the current resolution
        renderer.getSize(_v2A);
        const prevWidth = _v2A.x;
        const prevHeight = _v2A.y;
        // overwrite the resolution
        renderer.setSize(size, size, false);
        // assign the texture to plane
        _material.map = texture;
        // render
        renderer.render(_scene, _camera);
        // unassign the texture
        _material.map = null;
        // get blob
        if (canvas instanceof OffscreenCanvas) {
            return canvas.convertToBlob().finally(() => {
                // revert to previous resolution
                renderer.setSize(prevWidth, prevHeight, false);
            });
        }
        return new Promise((resolve, reject) => {
            canvas.toBlob((blob) => {
                // revert to previous resolution
                renderer.setSize(prevWidth, prevHeight, false);
                if (blob == null) {
                    reject('extractThumbnailBlob: Failed to create a blob');
                }
                else {
                    resolve(blob);
                }
            });
        });
    }

    /**
     * Traverse given object and remove unnecessarily bound joints from every `THREE.SkinnedMesh`.
     * Some environments like mobile devices have a lower limit of bones and might be unable to perform mesh skinning, this function might resolve such an issue.
     * Also this function might greatly improve the performance of mesh skinning.
     *
     * @param root Root object that will be traversed
     */
    function removeUnnecessaryJoints(root) {
        // some meshes might share a same skinIndex attribute and this map prevents to convert the attribute twice
        const skeletonList = new Map();
        // Traverse an entire tree
        root.traverse((obj) => {
            if (obj.type !== 'SkinnedMesh') {
                return;
            }
            const mesh = obj;
            const geometry = mesh.geometry;
            const attribute = geometry.getAttribute('skinIndex');
            // look for existing skeleton
            let skeleton = skeletonList.get(attribute);
            if (!skeleton) {
                // generate reduced bone list
                const bones = []; // new list of bone
                const boneInverses = []; // new list of boneInverse
                const boneIndexMap = {}; // map of old bone index vs. new bone index
                // create a new bone map
                const array = attribute.array;
                for (let i = 0; i < array.length; i++) {
                    const index = array[i];
                    // new skinIndex buffer
                    if (boneIndexMap[index] === undefined) {
                        boneIndexMap[index] = bones.length;
                        bones.push(mesh.skeleton.bones[index]);
                        boneInverses.push(mesh.skeleton.boneInverses[index]);
                    }
                    array[i] = boneIndexMap[index];
                }
                // replace with new indices
                attribute.copyArray(array);
                attribute.needsUpdate = true;
                // replace with new indices
                skeleton = new THREE.Skeleton(bones, boneInverses);
                skeletonList.set(attribute, skeleton);
            }
            mesh.bind(skeleton, new THREE.Matrix4());
            //                  ^^^^^^^^^^^^^^^^^^^ transform of meshes should be ignored
            // See: https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#skins
        });
    }

    class VRMUtils {
        constructor() {
            // this class is not meant to be instantiated
        }
    }
    VRMUtils.extractThumbnailBlob = extractThumbnailBlob;
    VRMUtils.removeUnnecessaryJoints = removeUnnecessaryJoints;

    const _v3$1 = new THREE.Vector3();
    class VRMLookAtHeadDebug extends VRMLookAtHead {
        setupHelper(scene, debugOption) {
            if (!debugOption.disableFaceDirectionHelper) {
                this._faceDirectionHelper = new THREE.ArrowHelper(new THREE.Vector3(0, 0, -1), new THREE.Vector3(0, 0, 0), 0.5, 0xff00ff);
                scene.add(this._faceDirectionHelper);
            }
        }
        update(delta) {
            super.update(delta);
            if (this._faceDirectionHelper) {
                this.firstPerson.getFirstPersonWorldPosition(this._faceDirectionHelper.position);
                this._faceDirectionHelper.setDirection(this.getLookAtWorldDirection(_v3$1));
            }
        }
    }

    class VRMLookAtImporterDebug extends VRMLookAtImporter {
        import(gltf, firstPerson, blendShapeProxy, humanoid) {
            var _a;
            const vrmExt = (_a = gltf.parser.json.extensions) === null || _a === void 0 ? void 0 : _a.VRM;
            if (!vrmExt) {
                return null;
            }
            const schemaFirstPerson = vrmExt.firstPerson;
            if (!schemaFirstPerson) {
                return null;
            }
            const applyer = this._importApplyer(schemaFirstPerson, blendShapeProxy, humanoid);
            return new VRMLookAtHeadDebug(firstPerson, applyer || undefined);
        }
    }

    const _colliderGizmoMaterial = new THREE.MeshBasicMaterial({
        color: 0xff00ff,
        wireframe: true,
        transparent: true,
        depthTest: false,
    });
    class VRMSpringBoneManagerDebug extends VRMSpringBoneManager {
        setupHelper(scene, debugOption) {
            if (debugOption.disableSpringBoneHelper)
                return;
            this.springBoneGroupList.forEach((springBoneGroup) => {
                springBoneGroup.forEach((springBone) => {
                    if (springBone.getGizmo) {
                        const gizmo = springBone.getGizmo();
                        scene.add(gizmo);
                    }
                });
            });
            this.colliderGroups.forEach((colliderGroup) => {
                colliderGroup.colliders.forEach((collider) => {
                    collider.material = _colliderGizmoMaterial;
                    collider.renderOrder = VRM_GIZMO_RENDER_ORDER;
                });
            });
        }
    }

    const _v3A$4 = new THREE.Vector3();
    class VRMSpringBoneDebug extends VRMSpringBone {
        constructor(bone, params) {
            super(bone, params);
        }
        /**
         * Return spring bone gizmo, as `THREE.ArrowHelper`.
         * Useful for debugging spring bones.
         */
        getGizmo() {
            // return if gizmo is already existed
            if (this._gizmo) {
                return this._gizmo;
            }
            const nextTailRelative = _v3A$4.copy(this._nextTail).sub(this._centerSpacePosition);
            const nextTailRelativeLength = nextTailRelative.length();
            this._gizmo = new THREE.ArrowHelper(nextTailRelative.normalize(), this._centerSpacePosition, nextTailRelativeLength, 0xffff00, this.radius, this.radius);
            // it should be always visible
            this._gizmo.line.renderOrder = VRM_GIZMO_RENDER_ORDER;
            this._gizmo.cone.renderOrder = VRM_GIZMO_RENDER_ORDER;
            this._gizmo.line.material.depthTest = false;
            this._gizmo.line.material.transparent = true;
            this._gizmo.cone.material.depthTest = false;
            this._gizmo.cone.material.transparent = true;
            return this._gizmo;
        }
        update(delta) {
            super.update(delta);
            // lastly we're gonna update gizmo
            this._updateGizmo();
        }
        _updateGizmo() {
            if (!this._gizmo) {
                return;
            }
            const nextTailRelative = _v3A$4.copy(this._currentTail).sub(this._centerSpacePosition);
            const nextTailRelativeLength = nextTailRelative.length();
            this._gizmo.setDirection(nextTailRelative.normalize());
            this._gizmo.setLength(nextTailRelativeLength, this.radius, this.radius);
            this._gizmo.position.copy(this._centerSpacePosition);
        }
    }

    class VRMSpringBoneImporterDebug extends VRMSpringBoneImporter {
        import(gltf) {
            var _a;
            return __awaiter(this, void 0, void 0, function* () {
                const vrmExt = (_a = gltf.parser.json.extensions) === null || _a === void 0 ? void 0 : _a.VRM;
                if (!vrmExt)
                    return null;
                const schemaSecondaryAnimation = vrmExt.secondaryAnimation;
                if (!schemaSecondaryAnimation)
                    return null;
                // 衝突判定球体メッシュ。
                const colliderGroups = yield this._importColliderMeshGroups(gltf, schemaSecondaryAnimation);
                // 同じ属性（stiffinessやdragForceが同じ）のボーンはboneGroupにまとめられている。
                // 一列だけではないことに注意。
                const springBoneGroupList = yield this._importSpringBoneGroupList(gltf, schemaSecondaryAnimation, colliderGroups);
                return new VRMSpringBoneManagerDebug(colliderGroups, springBoneGroupList);
            });
        }
        _createSpringBone(bone, params) {
            return new VRMSpringBoneDebug(bone, params);
        }
    }

    /**
     * An importer that imports a [[VRMDebug]] from a VRM extension of a GLTF.
     */
    class VRMImporterDebug extends VRMImporter {
        constructor(options = {}) {
            options.lookAtImporter = options.lookAtImporter || new VRMLookAtImporterDebug();
            options.springBoneImporter = options.springBoneImporter || new VRMSpringBoneImporterDebug();
            super(options);
        }
        import(gltf, debugOptions = {}) {
            return __awaiter(this, void 0, void 0, function* () {
                if (gltf.parser.json.extensions === undefined || gltf.parser.json.extensions.VRM === undefined) {
                    throw new Error('Could not find VRM extension on the GLTF');
                }
                const scene = gltf.scene;
                scene.updateMatrixWorld(false);
                // Skinned object should not be frustumCulled
                // Since pre-skinned position might be outside of view
                scene.traverse((object3d) => {
                    if (object3d.isMesh) {
                        object3d.frustumCulled = false;
                    }
                });
                const meta = (yield this._metaImporter.import(gltf)) || undefined;
                const materials = (yield this._materialImporter.convertGLTFMaterials(gltf)) || undefined;
                const humanoid = (yield this._humanoidImporter.import(gltf)) || undefined;
                const firstPerson = humanoid ? (yield this._firstPersonImporter.import(gltf, humanoid)) || undefined : undefined;
                const blendShapeProxy = (yield this._blendShapeImporter.import(gltf)) || undefined;
                const lookAt = firstPerson && blendShapeProxy && humanoid
                    ? (yield this._lookAtImporter.import(gltf, firstPerson, blendShapeProxy, humanoid)) || undefined
                    : undefined;
                if (lookAt.setupHelper) {
                    lookAt.setupHelper(scene, debugOptions);
                }
                const springBoneManager = (yield this._springBoneImporter.import(gltf)) || undefined;
                if (springBoneManager.setupHelper) {
                    springBoneManager.setupHelper(scene, debugOptions);
                }
                return new VRMDebug({
                    scene: gltf.scene,
                    meta,
                    materials,
                    humanoid,
                    firstPerson,
                    blendShapeProxy,
                    lookAt,
                    springBoneManager,
                }, debugOptions);
            });
        }
    }

    const VRM_GIZMO_RENDER_ORDER = 10000;
    /**
     * [[VRM]] but it has some useful gizmos.
     */
    class VRMDebug extends VRM {
        /**
         * Create a new VRMDebug from a parsed result of GLTF taken from GLTFLoader.
         *
         * See [[VRM.from]] for a detailed example.
         *
         * @param gltf A parsed GLTF object taken from GLTFLoader
         * @param options Options that will be used in importer
         * @param debugOption Options for VRMDebug features
         */
        static from(gltf, options = {}, debugOption = {}) {
            return __awaiter(this, void 0, void 0, function* () {
                const importer = new VRMImporterDebug(options);
                return yield importer.import(gltf, debugOption);
            });
        }
        /**
         * Create a new VRMDebug instance.
         *
         * @param params [[VRMParameters]] that represents components of the VRM
         * @param debugOption Options for VRMDebug features
         */
        constructor(params, debugOption = {}) {
            super(params);
            // Gizmoを展開
            if (!debugOption.disableBoxHelper) {
                this.scene.add(new THREE.BoxHelper(this.scene));
            }
            if (!debugOption.disableSkeletonHelper) {
                this.scene.add(new THREE.SkeletonHelper(this.scene));
            }
        }
        update(delta) {
            super.update(delta);
        }
    }

    exports.MToonMaterial = MToonMaterial;
    exports.VRM = VRM;
    exports.VRMBlendShapeGroup = VRMBlendShapeGroup;
    exports.VRMBlendShapeImporter = VRMBlendShapeImporter;
    exports.VRMBlendShapeProxy = VRMBlendShapeProxy;
    exports.VRMCurveMapper = VRMCurveMapper;
    exports.VRMDebug = VRMDebug;
    exports.VRMFirstPerson = VRMFirstPerson;
    exports.VRMFirstPersonImporter = VRMFirstPersonImporter;
    exports.VRMHumanBone = VRMHumanBone;
    exports.VRMHumanoid = VRMHumanoid;
    exports.VRMHumanoidImporter = VRMHumanoidImporter;
    exports.VRMImporter = VRMImporter;
    exports.VRMLookAtApplyer = VRMLookAtApplyer;
    exports.VRMLookAtBlendShapeApplyer = VRMLookAtBlendShapeApplyer;
    exports.VRMLookAtBoneApplyer = VRMLookAtBoneApplyer;
    exports.VRMLookAtHead = VRMLookAtHead;
    exports.VRMLookAtImporter = VRMLookAtImporter;
    exports.VRMMaterialImporter = VRMMaterialImporter;
    exports.VRMMetaImporter = VRMMetaImporter;
    exports.VRMRendererFirstPersonFlags = VRMRendererFirstPersonFlags;
    exports.VRMSpringBone = VRMSpringBone;
    exports.VRMSpringBoneDebug = VRMSpringBoneDebug;
    exports.VRMSpringBoneImporter = VRMSpringBoneImporter;
    exports.VRMSpringBoneImporterDebug = VRMSpringBoneImporterDebug;
    exports.VRMSpringBoneManager = VRMSpringBoneManager;
    exports.VRMUnlitMaterial = VRMUnlitMaterial;
    exports.VRMUtils = VRMUtils;
    exports.VRM_GIZMO_RENDER_ORDER = VRM_GIZMO_RENDER_ORDER;

    Object.defineProperty(exports, '__esModule', { value: true });

    Object.assign(THREE, exports);

})));
//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJmaWxlIjoidGhyZWUtdnJtLmpzIiwic291cmNlcyI6WyIuLi8uLi8uLi9ub2RlX21vZHVsZXMvdHNsaWIvdHNsaWIuZXM2LmpzIiwiLi4vc3JjL3V0aWxzL2Rpc3Bvc2VyLnRzIiwiLi4vc3JjL2JsZW5kc2hhcGUvVlJNQmxlbmRTaGFwZUdyb3VwLnRzIiwiLi4vc3JjL3R5cGVzL1ZSTVNjaGVtYS50cyIsIi4uL3NyYy91dGlscy9nbHRmRXh0cmFjdFByaW1pdGl2ZXNGcm9tTm9kZS50cyIsIi4uL3NyYy91dGlscy9yZW5hbWVNYXRlcmlhbFByb3BlcnR5LnRzIiwiLi4vc3JjL3V0aWxzL21hdGgudHMiLCIuLi9zcmMvYmxlbmRzaGFwZS9WUk1CbGVuZFNoYXBlUHJveHkudHMiLCIuLi9zcmMvYmxlbmRzaGFwZS9WUk1CbGVuZFNoYXBlSW1wb3J0ZXIudHMiLCIuLi9zcmMvZmlyc3RwZXJzb24vVlJNRmlyc3RQZXJzb24udHMiLCIuLi9zcmMvZmlyc3RwZXJzb24vVlJNRmlyc3RQZXJzb25JbXBvcnRlci50cyIsIi4uL3NyYy9odW1hbm9pZC9WUk1IdW1hbkJvbmUudHMiLCIuLi9zcmMvdXRpbHMvcXVhdEludmVydENvbXBhdC50cyIsIi4uL3NyYy9odW1hbm9pZC9WUk1IdW1hbm9pZC50cyIsIi4uL3NyYy9odW1hbm9pZC9WUk1IdW1hbm9pZEltcG9ydGVyLnRzIiwiLi4vc3JjL2xvb2thdC9WUk1DdXJ2ZU1hcHBlci50cyIsIi4uL3NyYy9sb29rYXQvVlJNTG9va0F0QXBwbHllci50cyIsIi4uL3NyYy9sb29rYXQvVlJNTG9va0F0QmxlbmRTaGFwZUFwcGx5ZXIudHMiLCIuLi9zcmMvbG9va2F0L1ZSTUxvb2tBdEhlYWQudHMiLCIuLi9zcmMvbG9va2F0L1ZSTUxvb2tBdEJvbmVBcHBseWVyLnRzIiwiLi4vc3JjL2xvb2thdC9WUk1Mb29rQXRJbXBvcnRlci50cyIsIi4uL3NyYy9tYXRlcmlhbC9nZXRUZXhlbERlY29kaW5nRnVuY3Rpb24udHMiLCIuLi9zcmMvbWF0ZXJpYWwvTVRvb25NYXRlcmlhbC50cyIsIi4uL3NyYy9tYXRlcmlhbC9WUk1VbmxpdE1hdGVyaWFsLnRzIiwiLi4vc3JjL21hdGVyaWFsL1ZSTU1hdGVyaWFsSW1wb3J0ZXIudHMiLCIuLi9zcmMvbWV0YS9WUk1NZXRhSW1wb3J0ZXIudHMiLCIuLi9zcmMvdXRpbHMvbWF0NEludmVydENvbXBhdC50cyIsIi4uL3NyYy91dGlscy9NYXRyaXg0SW52ZXJzZUNhY2hlLnRzIiwiLi4vc3JjL3NwcmluZ2JvbmUvVlJNU3ByaW5nQm9uZS50cyIsIi4uL3NyYy9zcHJpbmdib25lL1ZSTVNwcmluZ0JvbmVNYW5hZ2VyLnRzIiwiLi4vc3JjL3NwcmluZ2JvbmUvVlJNU3ByaW5nQm9uZUltcG9ydGVyLnRzIiwiLi4vc3JjL1ZSTUltcG9ydGVyLnRzIiwiLi4vc3JjL1ZSTS50cyIsIi4uL3NyYy9WUk1VdGlscy9leHRyYWN0VGh1bWJuYWlsQmxvYi50cyIsIi4uL3NyYy9WUk1VdGlscy9yZW1vdmVVbm5lY2Vzc2FyeUpvaW50cy50cyIsIi4uL3NyYy9WUk1VdGlscy9pbmRleC50cyIsIi4uL3NyYy9kZWJ1Zy9WUk1Mb29rQXRIZWFkRGVidWcudHMiLCIuLi9zcmMvZGVidWcvVlJNTG9va0F0SW1wb3J0ZXJEZWJ1Zy50cyIsIi4uL3NyYy9kZWJ1Zy9WUk1TcHJpbmdCb25lTWFuYWdlckRlYnVnLnRzIiwiLi4vc3JjL2RlYnVnL1ZSTVNwcmluZ0JvbmVEZWJ1Zy50cyIsIi4uL3NyYy9kZWJ1Zy9WUk1TcHJpbmdCb25lSW1wb3J0ZXJEZWJ1Zy50cyIsIi4uL3NyYy9kZWJ1Zy9WUk1JbXBvcnRlckRlYnVnLnRzIiwiLi4vc3JjL2RlYnVnL1ZSTURlYnVnLnRzIl0sInNvdXJjZXNDb250ZW50IjpbIi8qISAqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKlxyXG5Db3B5cmlnaHQgKGMpIE1pY3Jvc29mdCBDb3Jwb3JhdGlvbi5cclxuXHJcblBlcm1pc3Npb24gdG8gdXNlLCBjb3B5LCBtb2RpZnksIGFuZC9vciBkaXN0cmlidXRlIHRoaXMgc29mdHdhcmUgZm9yIGFueVxyXG5wdXJwb3NlIHdpdGggb3Igd2l0aG91dCBmZWUgaXMgaGVyZWJ5IGdyYW50ZWQuXHJcblxyXG5USEUgU09GVFdBUkUgSVMgUFJPVklERUQgXCJBUyBJU1wiIEFORCBUSEUgQVVUSE9SIERJU0NMQUlNUyBBTEwgV0FSUkFOVElFUyBXSVRIXHJcblJFR0FSRCBUTyBUSElTIFNPRlRXQVJFIElOQ0xVRElORyBBTEwgSU1QTElFRCBXQVJSQU5USUVTIE9GIE1FUkNIQU5UQUJJTElUWVxyXG5BTkQgRklUTkVTUy4gSU4gTk8gRVZFTlQgU0hBTEwgVEhFIEFVVEhPUiBCRSBMSUFCTEUgRk9SIEFOWSBTUEVDSUFMLCBESVJFQ1QsXHJcbklORElSRUNULCBPUiBDT05TRVFVRU5USUFMIERBTUFHRVMgT1IgQU5ZIERBTUFHRVMgV0hBVFNPRVZFUiBSRVNVTFRJTkcgRlJPTVxyXG5MT1NTIE9GIFVTRSwgREFUQSBPUiBQUk9GSVRTLCBXSEVUSEVSIElOIEFOIEFDVElPTiBPRiBDT05UUkFDVCwgTkVHTElHRU5DRSBPUlxyXG5PVEhFUiBUT1JUSU9VUyBBQ1RJT04sIEFSSVNJTkcgT1VUIE9GIE9SIElOIENPTk5FQ1RJT04gV0lUSCBUSEUgVVNFIE9SXHJcblBFUkZPUk1BTkNFIE9GIFRISVMgU09GVFdBUkUuXHJcbioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqICovXHJcbi8qIGdsb2JhbCBSZWZsZWN0LCBQcm9taXNlICovXHJcblxyXG52YXIgZXh0ZW5kU3RhdGljcyA9IGZ1bmN0aW9uKGQsIGIpIHtcclxuICAgIGV4dGVuZFN0YXRpY3MgPSBPYmplY3Quc2V0UHJvdG90eXBlT2YgfHxcclxuICAgICAgICAoeyBfX3Byb3RvX186IFtdIH0gaW5zdGFuY2VvZiBBcnJheSAmJiBmdW5jdGlvbiAoZCwgYikgeyBkLl9fcHJvdG9fXyA9IGI7IH0pIHx8XHJcbiAgICAgICAgZnVuY3Rpb24gKGQsIGIpIHsgZm9yICh2YXIgcCBpbiBiKSBpZiAoT2JqZWN0LnByb3RvdHlwZS5oYXNPd25Qcm9wZXJ0eS5jYWxsKGIsIHApKSBkW3BdID0gYltwXTsgfTtcclxuICAgIHJldHVybiBleHRlbmRTdGF0aWNzKGQsIGIpO1xyXG59O1xyXG5cclxuZXhwb3J0IGZ1bmN0aW9uIF9fZXh0ZW5kcyhkLCBiKSB7XHJcbiAgICBpZiAodHlwZW9mIGIgIT09IFwiZnVuY3Rpb25cIiAmJiBiICE9PSBudWxsKVxyXG4gICAgICAgIHRocm93IG5ldyBUeXBlRXJyb3IoXCJDbGFzcyBleHRlbmRzIHZhbHVlIFwiICsgU3RyaW5nKGIpICsgXCIgaXMgbm90IGEgY29uc3RydWN0b3Igb3IgbnVsbFwiKTtcclxuICAgIGV4dGVuZFN0YXRpY3MoZCwgYik7XHJcbiAgICBmdW5jdGlvbiBfXygpIHsgdGhpcy5jb25zdHJ1Y3RvciA9IGQ7IH1cclxuICAgIGQucHJvdG90eXBlID0gYiA9PT0gbnVsbCA/IE9iamVjdC5jcmVhdGUoYikgOiAoX18ucHJvdG90eXBlID0gYi5wcm90b3R5cGUsIG5ldyBfXygpKTtcclxufVxyXG5cclxuZXhwb3J0IHZhciBfX2Fzc2lnbiA9IGZ1bmN0aW9uKCkge1xyXG4gICAgX19hc3NpZ24gPSBPYmplY3QuYXNzaWduIHx8IGZ1bmN0aW9uIF9fYXNzaWduKHQpIHtcclxuICAgICAgICBmb3IgKHZhciBzLCBpID0gMSwgbiA9IGFyZ3VtZW50cy5sZW5ndGg7IGkgPCBuOyBpKyspIHtcclxuICAgICAgICAgICAgcyA9IGFyZ3VtZW50c1tpXTtcclxuICAgICAgICAgICAgZm9yICh2YXIgcCBpbiBzKSBpZiAoT2JqZWN0LnByb3RvdHlwZS5oYXNPd25Qcm9wZXJ0eS5jYWxsKHMsIHApKSB0W3BdID0gc1twXTtcclxuICAgICAgICB9XHJcbiAgICAgICAgcmV0dXJuIHQ7XHJcbiAgICB9XHJcbiAgICByZXR1cm4gX19hc3NpZ24uYXBwbHkodGhpcywgYXJndW1lbnRzKTtcclxufVxyXG5cclxuZXhwb3J0IGZ1bmN0aW9uIF9fcmVzdChzLCBlKSB7XHJcbiAgICB2YXIgdCA9IHt9O1xyXG4gICAgZm9yICh2YXIgcCBpbiBzKSBpZiAoT2JqZWN0LnByb3RvdHlwZS5oYXNPd25Qcm9wZXJ0eS5jYWxsKHMsIHApICYmIGUuaW5kZXhPZihwKSA8IDApXHJcbiAgICAgICAgdFtwXSA9IHNbcF07XHJcbiAgICBpZiAocyAhPSBudWxsICYmIHR5cGVvZiBPYmplY3QuZ2V0T3duUHJvcGVydHlTeW1ib2xzID09PSBcImZ1bmN0aW9uXCIpXHJcbiAgICAgICAgZm9yICh2YXIgaSA9IDAsIHAgPSBPYmplY3QuZ2V0T3duUHJvcGVydHlTeW1ib2xzKHMpOyBpIDwgcC5sZW5ndGg7IGkrKykge1xyXG4gICAgICAgICAgICBpZiAoZS5pbmRleE9mKHBbaV0pIDwgMCAmJiBPYmplY3QucHJvdG90eXBlLnByb3BlcnR5SXNFbnVtZXJhYmxlLmNhbGwocywgcFtpXSkpXHJcbiAgICAgICAgICAgICAgICB0W3BbaV1dID0gc1twW2ldXTtcclxuICAgICAgICB9XHJcbiAgICByZXR1cm4gdDtcclxufVxyXG5cclxuZXhwb3J0IGZ1bmN0aW9uIF9fZGVjb3JhdGUoZGVjb3JhdG9ycywgdGFyZ2V0LCBrZXksIGRlc2MpIHtcclxuICAgIHZhciBjID0gYXJndW1lbnRzLmxlbmd0aCwgciA9IGMgPCAzID8gdGFyZ2V0IDogZGVzYyA9PT0gbnVsbCA/IGRlc2MgPSBPYmplY3QuZ2V0T3duUHJvcGVydHlEZXNjcmlwdG9yKHRhcmdldCwga2V5KSA6IGRlc2MsIGQ7XHJcbiAgICBpZiAodHlwZW9mIFJlZmxlY3QgPT09IFwib2JqZWN0XCIgJiYgdHlwZW9mIFJlZmxlY3QuZGVjb3JhdGUgPT09IFwiZnVuY3Rpb25cIikgciA9IFJlZmxlY3QuZGVjb3JhdGUoZGVjb3JhdG9ycywgdGFyZ2V0LCBrZXksIGRlc2MpO1xyXG4gICAgZWxzZSBmb3IgKHZhciBpID0gZGVjb3JhdG9ycy5sZW5ndGggLSAxOyBpID49IDA7IGktLSkgaWYgKGQgPSBkZWNvcmF0b3JzW2ldKSByID0gKGMgPCAzID8gZChyKSA6IGMgPiAzID8gZCh0YXJnZXQsIGtleSwgcikgOiBkKHRhcmdldCwga2V5KSkgfHwgcjtcclxuICAgIHJldHVybiBjID4gMyAmJiByICYmIE9iamVjdC5kZWZpbmVQcm9wZXJ0eSh0YXJnZXQsIGtleSwgciksIHI7XHJcbn1cclxuXHJcbmV4cG9ydCBmdW5jdGlvbiBfX3BhcmFtKHBhcmFtSW5kZXgsIGRlY29yYXRvcikge1xyXG4gICAgcmV0dXJuIGZ1bmN0aW9uICh0YXJnZXQsIGtleSkgeyBkZWNvcmF0b3IodGFyZ2V0LCBrZXksIHBhcmFtSW5kZXgpOyB9XHJcbn1cclxuXHJcbmV4cG9ydCBmdW5jdGlvbiBfX21ldGFkYXRhKG1ldGFkYXRhS2V5LCBtZXRhZGF0YVZhbHVlKSB7XHJcbiAgICBpZiAodHlwZW9mIFJlZmxlY3QgPT09IFwib2JqZWN0XCIgJiYgdHlwZW9mIFJlZmxlY3QubWV0YWRhdGEgPT09IFwiZnVuY3Rpb25cIikgcmV0dXJuIFJlZmxlY3QubWV0YWRhdGEobWV0YWRhdGFLZXksIG1ldGFkYXRhVmFsdWUpO1xyXG59XHJcblxyXG5leHBvcnQgZnVuY3Rpb24gX19hd2FpdGVyKHRoaXNBcmcsIF9hcmd1bWVudHMsIFAsIGdlbmVyYXRvcikge1xyXG4gICAgZnVuY3Rpb24gYWRvcHQodmFsdWUpIHsgcmV0dXJuIHZhbHVlIGluc3RhbmNlb2YgUCA/IHZhbHVlIDogbmV3IFAoZnVuY3Rpb24gKHJlc29sdmUpIHsgcmVzb2x2ZSh2YWx1ZSk7IH0pOyB9XHJcbiAgICByZXR1cm4gbmV3IChQIHx8IChQID0gUHJvbWlzZSkpKGZ1bmN0aW9uIChyZXNvbHZlLCByZWplY3QpIHtcclxuICAgICAgICBmdW5jdGlvbiBmdWxmaWxsZWQodmFsdWUpIHsgdHJ5IHsgc3RlcChnZW5lcmF0b3IubmV4dCh2YWx1ZSkpOyB9IGNhdGNoIChlKSB7IHJlamVjdChlKTsgfSB9XHJcbiAgICAgICAgZnVuY3Rpb24gcmVqZWN0ZWQodmFsdWUpIHsgdHJ5IHsgc3RlcChnZW5lcmF0b3JbXCJ0aHJvd1wiXSh2YWx1ZSkpOyB9IGNhdGNoIChlKSB7IHJlamVjdChlKTsgfSB9XHJcbiAgICAgICAgZnVuY3Rpb24gc3RlcChyZXN1bHQpIHsgcmVzdWx0LmRvbmUgPyByZXNvbHZlKHJlc3VsdC52YWx1ZSkgOiBhZG9wdChyZXN1bHQudmFsdWUpLnRoZW4oZnVsZmlsbGVkLCByZWplY3RlZCk7IH1cclxuICAgICAgICBzdGVwKChnZW5lcmF0b3IgPSBnZW5lcmF0b3IuYXBwbHkodGhpc0FyZywgX2FyZ3VtZW50cyB8fCBbXSkpLm5leHQoKSk7XHJcbiAgICB9KTtcclxufVxyXG5cclxuZXhwb3J0IGZ1bmN0aW9uIF9fZ2VuZXJhdG9yKHRoaXNBcmcsIGJvZHkpIHtcclxuICAgIHZhciBfID0geyBsYWJlbDogMCwgc2VudDogZnVuY3Rpb24oKSB7IGlmICh0WzBdICYgMSkgdGhyb3cgdFsxXTsgcmV0dXJuIHRbMV07IH0sIHRyeXM6IFtdLCBvcHM6IFtdIH0sIGYsIHksIHQsIGc7XHJcbiAgICByZXR1cm4gZyA9IHsgbmV4dDogdmVyYigwKSwgXCJ0aHJvd1wiOiB2ZXJiKDEpLCBcInJldHVyblwiOiB2ZXJiKDIpIH0sIHR5cGVvZiBTeW1ib2wgPT09IFwiZnVuY3Rpb25cIiAmJiAoZ1tTeW1ib2wuaXRlcmF0b3JdID0gZnVuY3Rpb24oKSB7IHJldHVybiB0aGlzOyB9KSwgZztcclxuICAgIGZ1bmN0aW9uIHZlcmIobikgeyByZXR1cm4gZnVuY3Rpb24gKHYpIHsgcmV0dXJuIHN0ZXAoW24sIHZdKTsgfTsgfVxyXG4gICAgZnVuY3Rpb24gc3RlcChvcCkge1xyXG4gICAgICAgIGlmIChmKSB0aHJvdyBuZXcgVHlwZUVycm9yKFwiR2VuZXJhdG9yIGlzIGFscmVhZHkgZXhlY3V0aW5nLlwiKTtcclxuICAgICAgICB3aGlsZSAoXykgdHJ5IHtcclxuICAgICAgICAgICAgaWYgKGYgPSAxLCB5ICYmICh0ID0gb3BbMF0gJiAyID8geVtcInJldHVyblwiXSA6IG9wWzBdID8geVtcInRocm93XCJdIHx8ICgodCA9IHlbXCJyZXR1cm5cIl0pICYmIHQuY2FsbCh5KSwgMCkgOiB5Lm5leHQpICYmICEodCA9IHQuY2FsbCh5LCBvcFsxXSkpLmRvbmUpIHJldHVybiB0O1xyXG4gICAgICAgICAgICBpZiAoeSA9IDAsIHQpIG9wID0gW29wWzBdICYgMiwgdC52YWx1ZV07XHJcbiAgICAgICAgICAgIHN3aXRjaCAob3BbMF0pIHtcclxuICAgICAgICAgICAgICAgIGNhc2UgMDogY2FzZSAxOiB0ID0gb3A7IGJyZWFrO1xyXG4gICAgICAgICAgICAgICAgY2FzZSA0OiBfLmxhYmVsKys7IHJldHVybiB7IHZhbHVlOiBvcFsxXSwgZG9uZTogZmFsc2UgfTtcclxuICAgICAgICAgICAgICAgIGNhc2UgNTogXy5sYWJlbCsrOyB5ID0gb3BbMV07IG9wID0gWzBdOyBjb250aW51ZTtcclxuICAgICAgICAgICAgICAgIGNhc2UgNzogb3AgPSBfLm9wcy5wb3AoKTsgXy50cnlzLnBvcCgpOyBjb250aW51ZTtcclxuICAgICAgICAgICAgICAgIGRlZmF1bHQ6XHJcbiAgICAgICAgICAgICAgICAgICAgaWYgKCEodCA9IF8udHJ5cywgdCA9IHQubGVuZ3RoID4gMCAmJiB0W3QubGVuZ3RoIC0gMV0pICYmIChvcFswXSA9PT0gNiB8fCBvcFswXSA9PT0gMikpIHsgXyA9IDA7IGNvbnRpbnVlOyB9XHJcbiAgICAgICAgICAgICAgICAgICAgaWYgKG9wWzBdID09PSAzICYmICghdCB8fCAob3BbMV0gPiB0WzBdICYmIG9wWzFdIDwgdFszXSkpKSB7IF8ubGFiZWwgPSBvcFsxXTsgYnJlYWs7IH1cclxuICAgICAgICAgICAgICAgICAgICBpZiAob3BbMF0gPT09IDYgJiYgXy5sYWJlbCA8IHRbMV0pIHsgXy5sYWJlbCA9IHRbMV07IHQgPSBvcDsgYnJlYWs7IH1cclxuICAgICAgICAgICAgICAgICAgICBpZiAodCAmJiBfLmxhYmVsIDwgdFsyXSkgeyBfLmxhYmVsID0gdFsyXTsgXy5vcHMucHVzaChvcCk7IGJyZWFrOyB9XHJcbiAgICAgICAgICAgICAgICAgICAgaWYgKHRbMl0pIF8ub3BzLnBvcCgpO1xyXG4gICAgICAgICAgICAgICAgICAgIF8udHJ5cy5wb3AoKTsgY29udGludWU7XHJcbiAgICAgICAgICAgIH1cclxuICAgICAgICAgICAgb3AgPSBib2R5LmNhbGwodGhpc0FyZywgXyk7XHJcbiAgICAgICAgfSBjYXRjaCAoZSkgeyBvcCA9IFs2LCBlXTsgeSA9IDA7IH0gZmluYWxseSB7IGYgPSB0ID0gMDsgfVxyXG4gICAgICAgIGlmIChvcFswXSAmIDUpIHRocm93IG9wWzFdOyByZXR1cm4geyB2YWx1ZTogb3BbMF0gPyBvcFsxXSA6IHZvaWQgMCwgZG9uZTogdHJ1ZSB9O1xyXG4gICAgfVxyXG59XHJcblxyXG5leHBvcnQgdmFyIF9fY3JlYXRlQmluZGluZyA9IE9iamVjdC5jcmVhdGUgPyAoZnVuY3Rpb24obywgbSwgaywgazIpIHtcclxuICAgIGlmIChrMiA9PT0gdW5kZWZpbmVkKSBrMiA9IGs7XHJcbiAgICBPYmplY3QuZGVmaW5lUHJvcGVydHkobywgazIsIHsgZW51bWVyYWJsZTogdHJ1ZSwgZ2V0OiBmdW5jdGlvbigpIHsgcmV0dXJuIG1ba107IH0gfSk7XHJcbn0pIDogKGZ1bmN0aW9uKG8sIG0sIGssIGsyKSB7XHJcbiAgICBpZiAoazIgPT09IHVuZGVmaW5lZCkgazIgPSBrO1xyXG4gICAgb1trMl0gPSBtW2tdO1xyXG59KTtcclxuXHJcbmV4cG9ydCBmdW5jdGlvbiBfX2V4cG9ydFN0YXIobSwgbykge1xyXG4gICAgZm9yICh2YXIgcCBpbiBtKSBpZiAocCAhPT0gXCJkZWZhdWx0XCIgJiYgIU9iamVjdC5wcm90b3R5cGUuaGFzT3duUHJvcGVydHkuY2FsbChvLCBwKSkgX19jcmVhdGVCaW5kaW5nKG8sIG0sIHApO1xyXG59XHJcblxyXG5leHBvcnQgZnVuY3Rpb24gX192YWx1ZXMobykge1xyXG4gICAgdmFyIHMgPSB0eXBlb2YgU3ltYm9sID09PSBcImZ1bmN0aW9uXCIgJiYgU3ltYm9sLml0ZXJhdG9yLCBtID0gcyAmJiBvW3NdLCBpID0gMDtcclxuICAgIGlmIChtKSByZXR1cm4gbS5jYWxsKG8pO1xyXG4gICAgaWYgKG8gJiYgdHlwZW9mIG8ubGVuZ3RoID09PSBcIm51bWJlclwiKSByZXR1cm4ge1xyXG4gICAgICAgIG5leHQ6IGZ1bmN0aW9uICgpIHtcclxuICAgICAgICAgICAgaWYgKG8gJiYgaSA+PSBvLmxlbmd0aCkgbyA9IHZvaWQgMDtcclxuICAgICAgICAgICAgcmV0dXJuIHsgdmFsdWU6IG8gJiYgb1tpKytdLCBkb25lOiAhbyB9O1xyXG4gICAgICAgIH1cclxuICAgIH07XHJcbiAgICB0aHJvdyBuZXcgVHlwZUVycm9yKHMgPyBcIk9iamVjdCBpcyBub3QgaXRlcmFibGUuXCIgOiBcIlN5bWJvbC5pdGVyYXRvciBpcyBub3QgZGVmaW5lZC5cIik7XHJcbn1cclxuXHJcbmV4cG9ydCBmdW5jdGlvbiBfX3JlYWQobywgbikge1xyXG4gICAgdmFyIG0gPSB0eXBlb2YgU3ltYm9sID09PSBcImZ1bmN0aW9uXCIgJiYgb1tTeW1ib2wuaXRlcmF0b3JdO1xyXG4gICAgaWYgKCFtKSByZXR1cm4gbztcclxuICAgIHZhciBpID0gbS5jYWxsKG8pLCByLCBhciA9IFtdLCBlO1xyXG4gICAgdHJ5IHtcclxuICAgICAgICB3aGlsZSAoKG4gPT09IHZvaWQgMCB8fCBuLS0gPiAwKSAmJiAhKHIgPSBpLm5leHQoKSkuZG9uZSkgYXIucHVzaChyLnZhbHVlKTtcclxuICAgIH1cclxuICAgIGNhdGNoIChlcnJvcikgeyBlID0geyBlcnJvcjogZXJyb3IgfTsgfVxyXG4gICAgZmluYWxseSB7XHJcbiAgICAgICAgdHJ5IHtcclxuICAgICAgICAgICAgaWYgKHIgJiYgIXIuZG9uZSAmJiAobSA9IGlbXCJyZXR1cm5cIl0pKSBtLmNhbGwoaSk7XHJcbiAgICAgICAgfVxyXG4gICAgICAgIGZpbmFsbHkgeyBpZiAoZSkgdGhyb3cgZS5lcnJvcjsgfVxyXG4gICAgfVxyXG4gICAgcmV0dXJuIGFyO1xyXG59XHJcblxyXG4vKiogQGRlcHJlY2F0ZWQgKi9cclxuZXhwb3J0IGZ1bmN0aW9uIF9fc3ByZWFkKCkge1xyXG4gICAgZm9yICh2YXIgYXIgPSBbXSwgaSA9IDA7IGkgPCBhcmd1bWVudHMubGVuZ3RoOyBpKyspXHJcbiAgICAgICAgYXIgPSBhci5jb25jYXQoX19yZWFkKGFyZ3VtZW50c1tpXSkpO1xyXG4gICAgcmV0dXJuIGFyO1xyXG59XHJcblxyXG4vKiogQGRlcHJlY2F0ZWQgKi9cclxuZXhwb3J0IGZ1bmN0aW9uIF9fc3ByZWFkQXJyYXlzKCkge1xyXG4gICAgZm9yICh2YXIgcyA9IDAsIGkgPSAwLCBpbCA9IGFyZ3VtZW50cy5sZW5ndGg7IGkgPCBpbDsgaSsrKSBzICs9IGFyZ3VtZW50c1tpXS5sZW5ndGg7XHJcbiAgICBmb3IgKHZhciByID0gQXJyYXkocyksIGsgPSAwLCBpID0gMDsgaSA8IGlsOyBpKyspXHJcbiAgICAgICAgZm9yICh2YXIgYSA9IGFyZ3VtZW50c1tpXSwgaiA9IDAsIGpsID0gYS5sZW5ndGg7IGogPCBqbDsgaisrLCBrKyspXHJcbiAgICAgICAgICAgIHJba10gPSBhW2pdO1xyXG4gICAgcmV0dXJuIHI7XHJcbn1cclxuXHJcbmV4cG9ydCBmdW5jdGlvbiBfX3NwcmVhZEFycmF5KHRvLCBmcm9tKSB7XHJcbiAgICBmb3IgKHZhciBpID0gMCwgaWwgPSBmcm9tLmxlbmd0aCwgaiA9IHRvLmxlbmd0aDsgaSA8IGlsOyBpKyssIGorKylcclxuICAgICAgICB0b1tqXSA9IGZyb21baV07XHJcbiAgICByZXR1cm4gdG87XHJcbn1cclxuXHJcbmV4cG9ydCBmdW5jdGlvbiBfX2F3YWl0KHYpIHtcclxuICAgIHJldHVybiB0aGlzIGluc3RhbmNlb2YgX19hd2FpdCA/ICh0aGlzLnYgPSB2LCB0aGlzKSA6IG5ldyBfX2F3YWl0KHYpO1xyXG59XHJcblxyXG5leHBvcnQgZnVuY3Rpb24gX19hc3luY0dlbmVyYXRvcih0aGlzQXJnLCBfYXJndW1lbnRzLCBnZW5lcmF0b3IpIHtcclxuICAgIGlmICghU3ltYm9sLmFzeW5jSXRlcmF0b3IpIHRocm93IG5ldyBUeXBlRXJyb3IoXCJTeW1ib2wuYXN5bmNJdGVyYXRvciBpcyBub3QgZGVmaW5lZC5cIik7XHJcbiAgICB2YXIgZyA9IGdlbmVyYXRvci5hcHBseSh0aGlzQXJnLCBfYXJndW1lbnRzIHx8IFtdKSwgaSwgcSA9IFtdO1xyXG4gICAgcmV0dXJuIGkgPSB7fSwgdmVyYihcIm5leHRcIiksIHZlcmIoXCJ0aHJvd1wiKSwgdmVyYihcInJldHVyblwiKSwgaVtTeW1ib2wuYXN5bmNJdGVyYXRvcl0gPSBmdW5jdGlvbiAoKSB7IHJldHVybiB0aGlzOyB9LCBpO1xyXG4gICAgZnVuY3Rpb24gdmVyYihuKSB7IGlmIChnW25dKSBpW25dID0gZnVuY3Rpb24gKHYpIHsgcmV0dXJuIG5ldyBQcm9taXNlKGZ1bmN0aW9uIChhLCBiKSB7IHEucHVzaChbbiwgdiwgYSwgYl0pID4gMSB8fCByZXN1bWUobiwgdik7IH0pOyB9OyB9XHJcbiAgICBmdW5jdGlvbiByZXN1bWUobiwgdikgeyB0cnkgeyBzdGVwKGdbbl0odikpOyB9IGNhdGNoIChlKSB7IHNldHRsZShxWzBdWzNdLCBlKTsgfSB9XHJcbiAgICBmdW5jdGlvbiBzdGVwKHIpIHsgci52YWx1ZSBpbnN0YW5jZW9mIF9fYXdhaXQgPyBQcm9taXNlLnJlc29sdmUoci52YWx1ZS52KS50aGVuKGZ1bGZpbGwsIHJlamVjdCkgOiBzZXR0bGUocVswXVsyXSwgcik7IH1cclxuICAgIGZ1bmN0aW9uIGZ1bGZpbGwodmFsdWUpIHsgcmVzdW1lKFwibmV4dFwiLCB2YWx1ZSk7IH1cclxuICAgIGZ1bmN0aW9uIHJlamVjdCh2YWx1ZSkgeyByZXN1bWUoXCJ0aHJvd1wiLCB2YWx1ZSk7IH1cclxuICAgIGZ1bmN0aW9uIHNldHRsZShmLCB2KSB7IGlmIChmKHYpLCBxLnNoaWZ0KCksIHEubGVuZ3RoKSByZXN1bWUocVswXVswXSwgcVswXVsxXSk7IH1cclxufVxyXG5cclxuZXhwb3J0IGZ1bmN0aW9uIF9fYXN5bmNEZWxlZ2F0b3Iobykge1xyXG4gICAgdmFyIGksIHA7XHJcbiAgICByZXR1cm4gaSA9IHt9LCB2ZXJiKFwibmV4dFwiKSwgdmVyYihcInRocm93XCIsIGZ1bmN0aW9uIChlKSB7IHRocm93IGU7IH0pLCB2ZXJiKFwicmV0dXJuXCIpLCBpW1N5bWJvbC5pdGVyYXRvcl0gPSBmdW5jdGlvbiAoKSB7IHJldHVybiB0aGlzOyB9LCBpO1xyXG4gICAgZnVuY3Rpb24gdmVyYihuLCBmKSB7IGlbbl0gPSBvW25dID8gZnVuY3Rpb24gKHYpIHsgcmV0dXJuIChwID0gIXApID8geyB2YWx1ZTogX19hd2FpdChvW25dKHYpKSwgZG9uZTogbiA9PT0gXCJyZXR1cm5cIiB9IDogZiA/IGYodikgOiB2OyB9IDogZjsgfVxyXG59XHJcblxyXG5leHBvcnQgZnVuY3Rpb24gX19hc3luY1ZhbHVlcyhvKSB7XHJcbiAgICBpZiAoIVN5bWJvbC5hc3luY0l0ZXJhdG9yKSB0aHJvdyBuZXcgVHlwZUVycm9yKFwiU3ltYm9sLmFzeW5jSXRlcmF0b3IgaXMgbm90IGRlZmluZWQuXCIpO1xyXG4gICAgdmFyIG0gPSBvW1N5bWJvbC5hc3luY0l0ZXJhdG9yXSwgaTtcclxuICAgIHJldHVybiBtID8gbS5jYWxsKG8pIDogKG8gPSB0eXBlb2YgX192YWx1ZXMgPT09IFwiZnVuY3Rpb25cIiA/IF9fdmFsdWVzKG8pIDogb1tTeW1ib2wuaXRlcmF0b3JdKCksIGkgPSB7fSwgdmVyYihcIm5leHRcIiksIHZlcmIoXCJ0aHJvd1wiKSwgdmVyYihcInJldHVyblwiKSwgaVtTeW1ib2wuYXN5bmNJdGVyYXRvcl0gPSBmdW5jdGlvbiAoKSB7IHJldHVybiB0aGlzOyB9LCBpKTtcclxuICAgIGZ1bmN0aW9uIHZlcmIobikgeyBpW25dID0gb1tuXSAmJiBmdW5jdGlvbiAodikgeyByZXR1cm4gbmV3IFByb21pc2UoZnVuY3Rpb24gKHJlc29sdmUsIHJlamVjdCkgeyB2ID0gb1tuXSh2KSwgc2V0dGxlKHJlc29sdmUsIHJlamVjdCwgdi5kb25lLCB2LnZhbHVlKTsgfSk7IH07IH1cclxuICAgIGZ1bmN0aW9uIHNldHRsZShyZXNvbHZlLCByZWplY3QsIGQsIHYpIHsgUHJvbWlzZS5yZXNvbHZlKHYpLnRoZW4oZnVuY3Rpb24odikgeyByZXNvbHZlKHsgdmFsdWU6IHYsIGRvbmU6IGQgfSk7IH0sIHJlamVjdCk7IH1cclxufVxyXG5cclxuZXhwb3J0IGZ1bmN0aW9uIF9fbWFrZVRlbXBsYXRlT2JqZWN0KGNvb2tlZCwgcmF3KSB7XHJcbiAgICBpZiAoT2JqZWN0LmRlZmluZVByb3BlcnR5KSB7IE9iamVjdC5kZWZpbmVQcm9wZXJ0eShjb29rZWQsIFwicmF3XCIsIHsgdmFsdWU6IHJhdyB9KTsgfSBlbHNlIHsgY29va2VkLnJhdyA9IHJhdzsgfVxyXG4gICAgcmV0dXJuIGNvb2tlZDtcclxufTtcclxuXHJcbnZhciBfX3NldE1vZHVsZURlZmF1bHQgPSBPYmplY3QuY3JlYXRlID8gKGZ1bmN0aW9uKG8sIHYpIHtcclxuICAgIE9iamVjdC5kZWZpbmVQcm9wZXJ0eShvLCBcImRlZmF1bHRcIiwgeyBlbnVtZXJhYmxlOiB0cnVlLCB2YWx1ZTogdiB9KTtcclxufSkgOiBmdW5jdGlvbihvLCB2KSB7XHJcbiAgICBvW1wiZGVmYXVsdFwiXSA9IHY7XHJcbn07XHJcblxyXG5leHBvcnQgZnVuY3Rpb24gX19pbXBvcnRTdGFyKG1vZCkge1xyXG4gICAgaWYgKG1vZCAmJiBtb2QuX19lc01vZHVsZSkgcmV0dXJuIG1vZDtcclxuICAgIHZhciByZXN1bHQgPSB7fTtcclxuICAgIGlmIChtb2QgIT0gbnVsbCkgZm9yICh2YXIgayBpbiBtb2QpIGlmIChrICE9PSBcImRlZmF1bHRcIiAmJiBPYmplY3QucHJvdG90eXBlLmhhc093blByb3BlcnR5LmNhbGwobW9kLCBrKSkgX19jcmVhdGVCaW5kaW5nKHJlc3VsdCwgbW9kLCBrKTtcclxuICAgIF9fc2V0TW9kdWxlRGVmYXVsdChyZXN1bHQsIG1vZCk7XHJcbiAgICByZXR1cm4gcmVzdWx0O1xyXG59XHJcblxyXG5leHBvcnQgZnVuY3Rpb24gX19pbXBvcnREZWZhdWx0KG1vZCkge1xyXG4gICAgcmV0dXJuIChtb2QgJiYgbW9kLl9fZXNNb2R1bGUpID8gbW9kIDogeyBkZWZhdWx0OiBtb2QgfTtcclxufVxyXG5cclxuZXhwb3J0IGZ1bmN0aW9uIF9fY2xhc3NQcml2YXRlRmllbGRHZXQocmVjZWl2ZXIsIHByaXZhdGVNYXApIHtcclxuICAgIGlmICghcHJpdmF0ZU1hcC5oYXMocmVjZWl2ZXIpKSB7XHJcbiAgICAgICAgdGhyb3cgbmV3IFR5cGVFcnJvcihcImF0dGVtcHRlZCB0byBnZXQgcHJpdmF0ZSBmaWVsZCBvbiBub24taW5zdGFuY2VcIik7XHJcbiAgICB9XHJcbiAgICByZXR1cm4gcHJpdmF0ZU1hcC5nZXQocmVjZWl2ZXIpO1xyXG59XHJcblxyXG5leHBvcnQgZnVuY3Rpb24gX19jbGFzc1ByaXZhdGVGaWVsZFNldChyZWNlaXZlciwgcHJpdmF0ZU1hcCwgdmFsdWUpIHtcclxuICAgIGlmICghcHJpdmF0ZU1hcC5oYXMocmVjZWl2ZXIpKSB7XHJcbiAgICAgICAgdGhyb3cgbmV3IFR5cGVFcnJvcihcImF0dGVtcHRlZCB0byBzZXQgcHJpdmF0ZSBmaWVsZCBvbiBub24taW5zdGFuY2VcIik7XHJcbiAgICB9XHJcbiAgICBwcml2YXRlTWFwLnNldChyZWNlaXZlciwgdmFsdWUpO1xyXG4gICAgcmV0dXJuIHZhbHVlO1xyXG59XHJcbiIsIi8vIFNlZTogaHR0cHM6Ly90aHJlZWpzLm9yZy9kb2NzLyNtYW51YWwvZW4vaW50cm9kdWN0aW9uL0hvdy10by1kaXNwb3NlLW9mLW9iamVjdHNcblxuaW1wb3J0ICogYXMgVEhSRUUgZnJvbSAndGhyZWUnO1xuXG5mdW5jdGlvbiBkaXNwb3NlTWF0ZXJpYWwobWF0ZXJpYWw6IFRIUkVFLk1hdGVyaWFsKTogdm9pZCB7XG4gIE9iamVjdC5rZXlzKG1hdGVyaWFsKS5mb3JFYWNoKChwcm9wZXJ0eU5hbWUpID0+IHtcbiAgICBjb25zdCB2YWx1ZSA9IChtYXRlcmlhbCBhcyBhbnkpW3Byb3BlcnR5TmFtZV07XG4gICAgaWYgKHZhbHVlPy5pc1RleHR1cmUpIHtcbiAgICAgIGNvbnN0IHRleHR1cmUgPSB2YWx1ZSBhcyBUSFJFRS5UZXh0dXJlO1xuICAgICAgdGV4dHVyZS5kaXNwb3NlKCk7XG4gICAgfVxuICB9KTtcblxuICBtYXRlcmlhbC5kaXNwb3NlKCk7XG59XG5cbmZ1bmN0aW9uIGRpc3Bvc2Uob2JqZWN0M0Q6IFRIUkVFLk9iamVjdDNEKTogdm9pZCB7XG4gIGNvbnN0IGdlb21ldHJ5OiBUSFJFRS5CdWZmZXJHZW9tZXRyeSB8IHVuZGVmaW5lZCA9IChvYmplY3QzRCBhcyBhbnkpLmdlb21ldHJ5O1xuICBpZiAoZ2VvbWV0cnkpIHtcbiAgICBnZW9tZXRyeS5kaXNwb3NlKCk7XG4gIH1cblxuICBjb25zdCBtYXRlcmlhbDogVEhSRUUuTWF0ZXJpYWwgfCBUSFJFRS5NYXRlcmlhbFtdID0gKG9iamVjdDNEIGFzIGFueSkubWF0ZXJpYWw7XG4gIGlmIChtYXRlcmlhbCkge1xuICAgIGlmIChBcnJheS5pc0FycmF5KG1hdGVyaWFsKSkge1xuICAgICAgbWF0ZXJpYWwuZm9yRWFjaCgobWF0ZXJpYWw6IFRIUkVFLk1hdGVyaWFsKSA9PiBkaXNwb3NlTWF0ZXJpYWwobWF0ZXJpYWwpKTtcbiAgICB9IGVsc2UgaWYgKG1hdGVyaWFsKSB7XG4gICAgICBkaXNwb3NlTWF0ZXJpYWwobWF0ZXJpYWwpO1xuICAgIH1cbiAgfVxufVxuXG5leHBvcnQgZnVuY3Rpb24gZGVlcERpc3Bvc2Uob2JqZWN0M0Q6IFRIUkVFLk9iamVjdDNEKTogdm9pZCB7XG4gIG9iamVjdDNELnRyYXZlcnNlKGRpc3Bvc2UpO1xufVxuIiwiaW1wb3J0ICogYXMgVEhSRUUgZnJvbSAndGhyZWUnO1xuaW1wb3J0IHsgR0xURlByaW1pdGl2ZSB9IGZyb20gJy4uL3R5cGVzJztcblxuZXhwb3J0IGludGVyZmFjZSBWUk1CbGVuZFNoYXBlQmluZCB7XG4gIG1lc2hlczogR0xURlByaW1pdGl2ZVtdO1xuICBtb3JwaFRhcmdldEluZGV4OiBudW1iZXI7XG4gIHdlaWdodDogbnVtYmVyO1xufVxuXG5lbnVtIFZSTUJsZW5kU2hhcGVNYXRlcmlhbFZhbHVlVHlwZSB7XG4gIE5VTUJFUixcbiAgVkVDVE9SMixcbiAgVkVDVE9SMyxcbiAgVkVDVE9SNCxcbiAgQ09MT1IsXG59XG5cbmV4cG9ydCBpbnRlcmZhY2UgVlJNQmxlbmRTaGFwZU1hdGVyaWFsVmFsdWUge1xuICBtYXRlcmlhbDogVEhSRUUuTWF0ZXJpYWw7XG4gIHByb3BlcnR5TmFtZTogc3RyaW5nO1xuICBkZWZhdWx0VmFsdWU6IG51bWJlciB8IFRIUkVFLlZlY3RvcjIgfCBUSFJFRS5WZWN0b3IzIHwgVEhSRUUuVmVjdG9yNCB8IFRIUkVFLkNvbG9yO1xuICB0YXJnZXRWYWx1ZTogbnVtYmVyIHwgVEhSRUUuVmVjdG9yMiB8IFRIUkVFLlZlY3RvcjMgfCBUSFJFRS5WZWN0b3I0IHwgVEhSRUUuQ29sb3I7XG4gIGRlbHRhVmFsdWU6IG51bWJlciB8IFRIUkVFLlZlY3RvcjIgfCBUSFJFRS5WZWN0b3IzIHwgVEhSRUUuVmVjdG9yNCB8IFRIUkVFLkNvbG9yOyAvLyB0YXJnZXRWYWx1ZSAtIGRlZmF1bHRWYWx1ZVxuICB0eXBlOiBWUk1CbGVuZFNoYXBlTWF0ZXJpYWxWYWx1ZVR5cGU7XG59XG5cbmNvbnN0IF92MiA9IG5ldyBUSFJFRS5WZWN0b3IyKCk7XG5jb25zdCBfdjMgPSBuZXcgVEhSRUUuVmVjdG9yMygpO1xuY29uc3QgX3Y0ID0gbmV3IFRIUkVFLlZlY3RvcjQoKTtcbmNvbnN0IF9jb2xvciA9IG5ldyBUSFJFRS5Db2xvcigpO1xuXG4vLyBhbmltYXRpb25NaXhlciDjga7nm6Poppblr77osaHjga/jgIFTY2VuZSDjga7kuK3jgavlhaXjgaPjgabjgYTjgovlv4XopoHjgYzjgYLjgovjgIJcbi8vIOOBneOBruOBn+OCgeOAgeihqOekuuOCquODluOCuOOCp+OCr+ODiOOBp+OBr+OBquOBhOOBkeOCjOOBqeOAgU9iamVjdDNEIOOCkue2meaJv+OBl+OBpiBTY2VuZSDjgavmipXlhaXjgafjgY3jgovjgojjgYbjgavjgZnjgovjgIJcbmV4cG9ydCBjbGFzcyBWUk1CbGVuZFNoYXBlR3JvdXAgZXh0ZW5kcyBUSFJFRS5PYmplY3QzRCB7XG4gIHB1YmxpYyB3ZWlnaHQgPSAwLjA7XG4gIHB1YmxpYyBpc0JpbmFyeSA9IGZhbHNlO1xuXG4gIHByaXZhdGUgX2JpbmRzOiBWUk1CbGVuZFNoYXBlQmluZFtdID0gW107XG4gIHByaXZhdGUgX21hdGVyaWFsVmFsdWVzOiBWUk1CbGVuZFNoYXBlTWF0ZXJpYWxWYWx1ZVtdID0gW107XG5cbiAgY29uc3RydWN0b3IoZXhwcmVzc2lvbk5hbWU6IHN0cmluZykge1xuICAgIHN1cGVyKCk7XG4gICAgdGhpcy5uYW1lID0gYEJsZW5kU2hhcGVDb250cm9sbGVyXyR7ZXhwcmVzc2lvbk5hbWV9YDtcblxuICAgIC8vIHRyYXZlcnNlIOaZguOBruaVkea4iOaJi+auteOBqOOBl+OBpiBPYmplY3QzRCDjgafjga/jgarjgYTjgZPjgajjgpLmmI7npLrjgZfjgabjgYrjgY9cbiAgICB0aGlzLnR5cGUgPSAnQmxlbmRTaGFwZUNvbnRyb2xsZXInO1xuICAgIC8vIOihqOekuuebrueahOOBruOCquODluOCuOOCp+OCr+ODiOOBp+OBr+OBquOBhOOBruOBp+OAgeiyoOiNt+i7vea4m+OBruOBn+OCgeOBqyB2aXNpYmxlIOOCkiBmYWxzZSDjgavjgZfjgabjgYrjgY/jgIJcbiAgICAvLyDjgZPjgozjgavjgojjgorjgIHjgZPjga7jgqTjg7Pjgrnjgr/jg7Pjgrnjgavlr77jgZnjgovmr47jg5Xjg6zjg7zjg6Djga4gbWF0cml4IOiHquWLleioiOeul+OCkuecgeeVpeOBp+OBjeOCi+OAglxuICAgIHRoaXMudmlzaWJsZSA9IGZhbHNlO1xuICB9XG5cbiAgcHVibGljIGFkZEJpbmQoYXJnczogeyBtZXNoZXM6IEdMVEZQcmltaXRpdmVbXTsgbW9ycGhUYXJnZXRJbmRleDogbnVtYmVyOyB3ZWlnaHQ6IG51bWJlciB9KTogdm9pZCB7XG4gICAgLy8gb3JpZ2luYWwgd2VpZ2h0IGlzIDAtMTAwIGJ1dCB3ZSB3YW50IHRvIGRlYWwgd2l0aCB0aGlzIHZhbHVlIHdpdGhpbiAwLTFcbiAgICBjb25zdCB3ZWlnaHQgPSBhcmdzLndlaWdodCAvIDEwMDtcblxuICAgIHRoaXMuX2JpbmRzLnB1c2goe1xuICAgICAgbWVzaGVzOiBhcmdzLm1lc2hlcyxcbiAgICAgIG1vcnBoVGFyZ2V0SW5kZXg6IGFyZ3MubW9ycGhUYXJnZXRJbmRleCxcbiAgICAgIHdlaWdodCxcbiAgICB9KTtcbiAgfVxuXG4gIHB1YmxpYyBhZGRNYXRlcmlhbFZhbHVlKGFyZ3M6IHtcbiAgICBtYXRlcmlhbDogVEhSRUUuTWF0ZXJpYWw7XG4gICAgcHJvcGVydHlOYW1lOiBzdHJpbmc7XG4gICAgdGFyZ2V0VmFsdWU6IG51bWJlcltdO1xuICAgIGRlZmF1bHRWYWx1ZT86IG51bWJlciB8IFRIUkVFLlZlY3RvcjIgfCBUSFJFRS5WZWN0b3IzIHwgVEhSRUUuVmVjdG9yNCB8IFRIUkVFLkNvbG9yO1xuICB9KTogdm9pZCB7XG4gICAgY29uc3QgbWF0ZXJpYWwgPSBhcmdzLm1hdGVyaWFsO1xuICAgIGNvbnN0IHByb3BlcnR5TmFtZSA9IGFyZ3MucHJvcGVydHlOYW1lO1xuXG4gICAgbGV0IHZhbHVlID0gKG1hdGVyaWFsIGFzIGFueSlbcHJvcGVydHlOYW1lXTtcbiAgICBpZiAoIXZhbHVlKSB7XG4gICAgICAvLyBwcm9wZXJ0eSBoYXMgbm90IGJlZW4gZm91bmRcbiAgICAgIHJldHVybjtcbiAgICB9XG4gICAgdmFsdWUgPSBhcmdzLmRlZmF1bHRWYWx1ZSB8fCB2YWx1ZTtcblxuICAgIGxldCB0eXBlOiBWUk1CbGVuZFNoYXBlTWF0ZXJpYWxWYWx1ZVR5cGU7XG4gICAgbGV0IGRlZmF1bHRWYWx1ZTogbnVtYmVyIHwgVEhSRUUuVmVjdG9yMiB8IFRIUkVFLlZlY3RvcjMgfCBUSFJFRS5WZWN0b3I0IHwgVEhSRUUuQ29sb3I7XG4gICAgbGV0IHRhcmdldFZhbHVlOiBudW1iZXIgfCBUSFJFRS5WZWN0b3IyIHwgVEhSRUUuVmVjdG9yMyB8IFRIUkVFLlZlY3RvcjQgfCBUSFJFRS5Db2xvcjtcbiAgICBsZXQgZGVsdGFWYWx1ZTogbnVtYmVyIHwgVEhSRUUuVmVjdG9yMiB8IFRIUkVFLlZlY3RvcjMgfCBUSFJFRS5WZWN0b3I0IHwgVEhSRUUuQ29sb3I7XG5cbiAgICBpZiAoKHZhbHVlIGFzIGFueSkuaXNWZWN0b3IyKSB7XG4gICAgICB0eXBlID0gVlJNQmxlbmRTaGFwZU1hdGVyaWFsVmFsdWVUeXBlLlZFQ1RPUjI7XG4gICAgICBkZWZhdWx0VmFsdWUgPSAodmFsdWUgYXMgVEhSRUUuVmVjdG9yMikuY2xvbmUoKTtcbiAgICAgIHRhcmdldFZhbHVlID0gbmV3IFRIUkVFLlZlY3RvcjIoKS5mcm9tQXJyYXkoYXJncy50YXJnZXRWYWx1ZSk7XG4gICAgICBkZWx0YVZhbHVlID0gdGFyZ2V0VmFsdWUuY2xvbmUoKS5zdWIoZGVmYXVsdFZhbHVlKTtcbiAgICB9IGVsc2UgaWYgKCh2YWx1ZSBhcyBhbnkpLmlzVmVjdG9yMykge1xuICAgICAgdHlwZSA9IFZSTUJsZW5kU2hhcGVNYXRlcmlhbFZhbHVlVHlwZS5WRUNUT1IzO1xuICAgICAgZGVmYXVsdFZhbHVlID0gKHZhbHVlIGFzIFRIUkVFLlZlY3RvcjMpLmNsb25lKCk7XG4gICAgICB0YXJnZXRWYWx1ZSA9IG5ldyBUSFJFRS5WZWN0b3IzKCkuZnJvbUFycmF5KGFyZ3MudGFyZ2V0VmFsdWUpO1xuICAgICAgZGVsdGFWYWx1ZSA9IHRhcmdldFZhbHVlLmNsb25lKCkuc3ViKGRlZmF1bHRWYWx1ZSk7XG4gICAgfSBlbHNlIGlmICgodmFsdWUgYXMgYW55KS5pc1ZlY3RvcjQpIHtcbiAgICAgIHR5cGUgPSBWUk1CbGVuZFNoYXBlTWF0ZXJpYWxWYWx1ZVR5cGUuVkVDVE9SNDtcbiAgICAgIGRlZmF1bHRWYWx1ZSA9ICh2YWx1ZSBhcyBUSFJFRS5WZWN0b3I0KS5jbG9uZSgpO1xuXG4gICAgICAvLyB2ZWN0b3JQcm9wZXJ0eSBhbmQgdGFyZ2V0VmFsdWUgaW5kZXggaXMgZGlmZmVyZW50IGZyb20gZWFjaCBvdGhlclxuICAgICAgLy8gZXhwb3J0ZWQgdnJtIGJ5IFVuaVZSTSBmaWxlIGlzXG4gICAgICAvL1xuICAgICAgLy8gdmVjdG9yUHJvcGVydHlcbiAgICAgIC8vIG9mZnNldCA9IHRhcmdldFZhbHVlWzBdLCB0YXJnZXRWYWx1ZVsxXVxuICAgICAgLy8gdGlsaW5nID0gdGFyZ2V0VmFsdWVbMl0sIHRhcmdldFZhbHVlWzNdXG4gICAgICAvL1xuICAgICAgLy8gdGFyZ2V0VmFsdWVcbiAgICAgIC8vIG9mZnNldCA9IHRhcmdldFZhbHVlWzJdLCB0YXJnZXRWYWx1ZVszXVxuICAgICAgLy8gdGlsaW5nID0gdGFyZ2V0VmFsdWVbMF0sIHRhcmdldFZhbHVlWzFdXG4gICAgICB0YXJnZXRWYWx1ZSA9IG5ldyBUSFJFRS5WZWN0b3I0KCkuZnJvbUFycmF5KFtcbiAgICAgICAgYXJncy50YXJnZXRWYWx1ZVsyXSxcbiAgICAgICAgYXJncy50YXJnZXRWYWx1ZVszXSxcbiAgICAgICAgYXJncy50YXJnZXRWYWx1ZVswXSxcbiAgICAgICAgYXJncy50YXJnZXRWYWx1ZVsxXSxcbiAgICAgIF0pO1xuICAgICAgZGVsdGFWYWx1ZSA9IHRhcmdldFZhbHVlLmNsb25lKCkuc3ViKGRlZmF1bHRWYWx1ZSk7XG4gICAgfSBlbHNlIGlmICgodmFsdWUgYXMgYW55KS5pc0NvbG9yKSB7XG4gICAgICB0eXBlID0gVlJNQmxlbmRTaGFwZU1hdGVyaWFsVmFsdWVUeXBlLkNPTE9SO1xuICAgICAgZGVmYXVsdFZhbHVlID0gKHZhbHVlIGFzIFRIUkVFLkNvbG9yKS5jbG9uZSgpO1xuICAgICAgdGFyZ2V0VmFsdWUgPSBuZXcgVEhSRUUuQ29sb3IoKS5mcm9tQXJyYXkoYXJncy50YXJnZXRWYWx1ZSk7XG4gICAgICBkZWx0YVZhbHVlID0gdGFyZ2V0VmFsdWUuY2xvbmUoKS5zdWIoZGVmYXVsdFZhbHVlKTtcbiAgICB9IGVsc2Uge1xuICAgICAgdHlwZSA9IFZSTUJsZW5kU2hhcGVNYXRlcmlhbFZhbHVlVHlwZS5OVU1CRVI7XG4gICAgICBkZWZhdWx0VmFsdWUgPSB2YWx1ZSBhcyBudW1iZXI7XG4gICAgICB0YXJnZXRWYWx1ZSA9IGFyZ3MudGFyZ2V0VmFsdWVbMF07XG4gICAgICBkZWx0YVZhbHVlID0gdGFyZ2V0VmFsdWUgLSBkZWZhdWx0VmFsdWU7XG4gICAgfVxuXG4gICAgdGhpcy5fbWF0ZXJpYWxWYWx1ZXMucHVzaCh7XG4gICAgICBtYXRlcmlhbCxcbiAgICAgIHByb3BlcnR5TmFtZSxcbiAgICAgIGRlZmF1bHRWYWx1ZSxcbiAgICAgIHRhcmdldFZhbHVlLFxuICAgICAgZGVsdGFWYWx1ZSxcbiAgICAgIHR5cGUsXG4gICAgfSk7XG4gIH1cblxuICAvKipcbiAgICogQXBwbHkgd2VpZ2h0IHRvIGV2ZXJ5IGFzc2lnbmVkIGJsZW5kIHNoYXBlcy5cbiAgICogU2hvdWxkIGJlIGNhbGxlZCB2aWEge0BsaW5rIEJsZW5kU2hhcGVNYXN0ZXIjdXBkYXRlfS5cbiAgICovXG4gIHB1YmxpYyBhcHBseVdlaWdodCgpOiB2b2lkIHtcbiAgICBjb25zdCB3ID0gdGhpcy5pc0JpbmFyeSA/ICh0aGlzLndlaWdodCA8IDAuNSA/IDAuMCA6IDEuMCkgOiB0aGlzLndlaWdodDtcblxuICAgIHRoaXMuX2JpbmRzLmZvckVhY2goKGJpbmQpID0+IHtcbiAgICAgIGJpbmQubWVzaGVzLmZvckVhY2goKG1lc2gpID0+IHtcbiAgICAgICAgaWYgKCFtZXNoLm1vcnBoVGFyZ2V0SW5mbHVlbmNlcykge1xuICAgICAgICAgIHJldHVybjtcbiAgICAgICAgfSAvLyBUT0RPOiB3ZSBzaG91bGQga2ljayB0aGlzIGF0IGBhZGRCaW5kYFxuICAgICAgICBtZXNoLm1vcnBoVGFyZ2V0SW5mbHVlbmNlc1tiaW5kLm1vcnBoVGFyZ2V0SW5kZXhdICs9IHcgKiBiaW5kLndlaWdodDtcbiAgICAgIH0pO1xuICAgIH0pO1xuXG4gICAgdGhpcy5fbWF0ZXJpYWxWYWx1ZXMuZm9yRWFjaCgobWF0ZXJpYWxWYWx1ZSkgPT4ge1xuICAgICAgY29uc3QgcHJvcCA9IChtYXRlcmlhbFZhbHVlLm1hdGVyaWFsIGFzIGFueSlbbWF0ZXJpYWxWYWx1ZS5wcm9wZXJ0eU5hbWVdO1xuICAgICAgaWYgKHByb3AgPT09IHVuZGVmaW5lZCkge1xuICAgICAgICByZXR1cm47XG4gICAgICB9IC8vIFRPRE86IHdlIHNob3VsZCBraWNrIHRoaXMgYXQgYGFkZE1hdGVyaWFsVmFsdWVgXG5cbiAgICAgIGlmIChtYXRlcmlhbFZhbHVlLnR5cGUgPT09IFZSTUJsZW5kU2hhcGVNYXRlcmlhbFZhbHVlVHlwZS5OVU1CRVIpIHtcbiAgICAgICAgY29uc3QgZGVsdGFWYWx1ZSA9IG1hdGVyaWFsVmFsdWUuZGVsdGFWYWx1ZSBhcyBudW1iZXI7XG4gICAgICAgIChtYXRlcmlhbFZhbHVlLm1hdGVyaWFsIGFzIGFueSlbbWF0ZXJpYWxWYWx1ZS5wcm9wZXJ0eU5hbWVdICs9IGRlbHRhVmFsdWUgKiB3O1xuICAgICAgfSBlbHNlIGlmIChtYXRlcmlhbFZhbHVlLnR5cGUgPT09IFZSTUJsZW5kU2hhcGVNYXRlcmlhbFZhbHVlVHlwZS5WRUNUT1IyKSB7XG4gICAgICAgIGNvbnN0IGRlbHRhVmFsdWUgPSBtYXRlcmlhbFZhbHVlLmRlbHRhVmFsdWUgYXMgVEhSRUUuVmVjdG9yMjtcbiAgICAgICAgKG1hdGVyaWFsVmFsdWUubWF0ZXJpYWwgYXMgYW55KVttYXRlcmlhbFZhbHVlLnByb3BlcnR5TmFtZV0uYWRkKF92Mi5jb3B5KGRlbHRhVmFsdWUpLm11bHRpcGx5U2NhbGFyKHcpKTtcbiAgICAgIH0gZWxzZSBpZiAobWF0ZXJpYWxWYWx1ZS50eXBlID09PSBWUk1CbGVuZFNoYXBlTWF0ZXJpYWxWYWx1ZVR5cGUuVkVDVE9SMykge1xuICAgICAgICBjb25zdCBkZWx0YVZhbHVlID0gbWF0ZXJpYWxWYWx1ZS5kZWx0YVZhbHVlIGFzIFRIUkVFLlZlY3RvcjM7XG4gICAgICAgIChtYXRlcmlhbFZhbHVlLm1hdGVyaWFsIGFzIGFueSlbbWF0ZXJpYWxWYWx1ZS5wcm9wZXJ0eU5hbWVdLmFkZChfdjMuY29weShkZWx0YVZhbHVlKS5tdWx0aXBseVNjYWxhcih3KSk7XG4gICAgICB9IGVsc2UgaWYgKG1hdGVyaWFsVmFsdWUudHlwZSA9PT0gVlJNQmxlbmRTaGFwZU1hdGVyaWFsVmFsdWVUeXBlLlZFQ1RPUjQpIHtcbiAgICAgICAgY29uc3QgZGVsdGFWYWx1ZSA9IG1hdGVyaWFsVmFsdWUuZGVsdGFWYWx1ZSBhcyBUSFJFRS5WZWN0b3I0O1xuICAgICAgICAobWF0ZXJpYWxWYWx1ZS5tYXRlcmlhbCBhcyBhbnkpW21hdGVyaWFsVmFsdWUucHJvcGVydHlOYW1lXS5hZGQoX3Y0LmNvcHkoZGVsdGFWYWx1ZSkubXVsdGlwbHlTY2FsYXIodykpO1xuICAgICAgfSBlbHNlIGlmIChtYXRlcmlhbFZhbHVlLnR5cGUgPT09IFZSTUJsZW5kU2hhcGVNYXRlcmlhbFZhbHVlVHlwZS5DT0xPUikge1xuICAgICAgICBjb25zdCBkZWx0YVZhbHVlID0gbWF0ZXJpYWxWYWx1ZS5kZWx0YVZhbHVlIGFzIFRIUkVFLkNvbG9yO1xuICAgICAgICAobWF0ZXJpYWxWYWx1ZS5tYXRlcmlhbCBhcyBhbnkpW21hdGVyaWFsVmFsdWUucHJvcGVydHlOYW1lXS5hZGQoX2NvbG9yLmNvcHkoZGVsdGFWYWx1ZSkubXVsdGlwbHlTY2FsYXIodykpO1xuICAgICAgfVxuXG4gICAgICBpZiAodHlwZW9mIChtYXRlcmlhbFZhbHVlLm1hdGVyaWFsIGFzIGFueSkuc2hvdWxkQXBwbHlVbmlmb3JtcyA9PT0gJ2Jvb2xlYW4nKSB7XG4gICAgICAgIChtYXRlcmlhbFZhbHVlLm1hdGVyaWFsIGFzIGFueSkuc2hvdWxkQXBwbHlVbmlmb3JtcyA9IHRydWU7XG4gICAgICB9XG4gICAgfSk7XG4gIH1cblxuICAvKipcbiAgICogQ2xlYXIgcHJldmlvdXNseSBhc3NpZ25lZCBibGVuZCBzaGFwZXMuXG4gICAqL1xuICBwdWJsaWMgY2xlYXJBcHBsaWVkV2VpZ2h0KCk6IHZvaWQge1xuICAgIHRoaXMuX2JpbmRzLmZvckVhY2goKGJpbmQpID0+IHtcbiAgICAgIGJpbmQubWVzaGVzLmZvckVhY2goKG1lc2gpID0+IHtcbiAgICAgICAgaWYgKCFtZXNoLm1vcnBoVGFyZ2V0SW5mbHVlbmNlcykge1xuICAgICAgICAgIHJldHVybjtcbiAgICAgICAgfSAvLyBUT0RPOiB3ZSBzaG91bGQga2ljayB0aGlzIGF0IGBhZGRCaW5kYFxuICAgICAgICBtZXNoLm1vcnBoVGFyZ2V0SW5mbHVlbmNlc1tiaW5kLm1vcnBoVGFyZ2V0SW5kZXhdID0gMC4wO1xuICAgICAgfSk7XG4gICAgfSk7XG5cbiAgICB0aGlzLl9tYXRlcmlhbFZhbHVlcy5mb3JFYWNoKChtYXRlcmlhbFZhbHVlKSA9PiB7XG4gICAgICBjb25zdCBwcm9wID0gKG1hdGVyaWFsVmFsdWUubWF0ZXJpYWwgYXMgYW55KVttYXRlcmlhbFZhbHVlLnByb3BlcnR5TmFtZV07XG4gICAgICBpZiAocHJvcCA9PT0gdW5kZWZpbmVkKSB7XG4gICAgICAgIHJldHVybjtcbiAgICAgIH0gLy8gVE9ETzogd2Ugc2hvdWxkIGtpY2sgdGhpcyBhdCBgYWRkTWF0ZXJpYWxWYWx1ZWBcblxuICAgICAgaWYgKG1hdGVyaWFsVmFsdWUudHlwZSA9PT0gVlJNQmxlbmRTaGFwZU1hdGVyaWFsVmFsdWVUeXBlLk5VTUJFUikge1xuICAgICAgICBjb25zdCBkZWZhdWx0VmFsdWUgPSBtYXRlcmlhbFZhbHVlLmRlZmF1bHRWYWx1ZSBhcyBudW1iZXI7XG4gICAgICAgIChtYXRlcmlhbFZhbHVlLm1hdGVyaWFsIGFzIGFueSlbbWF0ZXJpYWxWYWx1ZS5wcm9wZXJ0eU5hbWVdID0gZGVmYXVsdFZhbHVlO1xuICAgICAgfSBlbHNlIGlmIChtYXRlcmlhbFZhbHVlLnR5cGUgPT09IFZSTUJsZW5kU2hhcGVNYXRlcmlhbFZhbHVlVHlwZS5WRUNUT1IyKSB7XG4gICAgICAgIGNvbnN0IGRlZmF1bHRWYWx1ZSA9IG1hdGVyaWFsVmFsdWUuZGVmYXVsdFZhbHVlIGFzIFRIUkVFLlZlY3RvcjI7XG4gICAgICAgIChtYXRlcmlhbFZhbHVlLm1hdGVyaWFsIGFzIGFueSlbbWF0ZXJpYWxWYWx1ZS5wcm9wZXJ0eU5hbWVdLmNvcHkoZGVmYXVsdFZhbHVlKTtcbiAgICAgIH0gZWxzZSBpZiAobWF0ZXJpYWxWYWx1ZS50eXBlID09PSBWUk1CbGVuZFNoYXBlTWF0ZXJpYWxWYWx1ZVR5cGUuVkVDVE9SMykge1xuICAgICAgICBjb25zdCBkZWZhdWx0VmFsdWUgPSBtYXRlcmlhbFZhbHVlLmRlZmF1bHRWYWx1ZSBhcyBUSFJFRS5WZWN0b3IzO1xuICAgICAgICAobWF0ZXJpYWxWYWx1ZS5tYXRlcmlhbCBhcyBhbnkpW21hdGVyaWFsVmFsdWUucHJvcGVydHlOYW1lXS5jb3B5KGRlZmF1bHRWYWx1ZSk7XG4gICAgICB9IGVsc2UgaWYgKG1hdGVyaWFsVmFsdWUudHlwZSA9PT0gVlJNQmxlbmRTaGFwZU1hdGVyaWFsVmFsdWVUeXBlLlZFQ1RPUjQpIHtcbiAgICAgICAgY29uc3QgZGVmYXVsdFZhbHVlID0gbWF0ZXJpYWxWYWx1ZS5kZWZhdWx0VmFsdWUgYXMgVEhSRUUuVmVjdG9yNDtcbiAgICAgICAgKG1hdGVyaWFsVmFsdWUubWF0ZXJpYWwgYXMgYW55KVttYXRlcmlhbFZhbHVlLnByb3BlcnR5TmFtZV0uY29weShkZWZhdWx0VmFsdWUpO1xuICAgICAgfSBlbHNlIGlmIChtYXRlcmlhbFZhbHVlLnR5cGUgPT09IFZSTUJsZW5kU2hhcGVNYXRlcmlhbFZhbHVlVHlwZS5DT0xPUikge1xuICAgICAgICBjb25zdCBkZWZhdWx0VmFsdWUgPSBtYXRlcmlhbFZhbHVlLmRlZmF1bHRWYWx1ZSBhcyBUSFJFRS5Db2xvcjtcbiAgICAgICAgKG1hdGVyaWFsVmFsdWUubWF0ZXJpYWwgYXMgYW55KVttYXRlcmlhbFZhbHVlLnByb3BlcnR5TmFtZV0uY29weShkZWZhdWx0VmFsdWUpO1xuICAgICAgfVxuXG4gICAgICBpZiAodHlwZW9mIChtYXRlcmlhbFZhbHVlLm1hdGVyaWFsIGFzIGFueSkuc2hvdWxkQXBwbHlVbmlmb3JtcyA9PT0gJ2Jvb2xlYW4nKSB7XG4gICAgICAgIChtYXRlcmlhbFZhbHVlLm1hdGVyaWFsIGFzIGFueSkuc2hvdWxkQXBwbHlVbmlmb3JtcyA9IHRydWU7XG4gICAgICB9XG4gICAgfSk7XG4gIH1cbn1cbiIsIi8vIFR5cGVkb2MgZG9lcyBub3Qgc3VwcG9ydCBleHBvcnQgZGVjbGFyYXRpb25zIHlldFxuLy8gdGhlbiB3ZSBoYXZlIHRvIHVzZSBgbmFtZXNwYWNlYCBpbnN0ZWFkIG9mIGV4cG9ydCBkZWNsYXJhdGlvbnMgZm9yIG5vdy5cbi8vIFNlZTogaHR0cHM6Ly9naXRodWIuY29tL1R5cGVTdHJvbmcvdHlwZWRvYy9wdWxsLzgwMVxuXG4vLyBlc2xpbnQtZGlzYWJsZS1uZXh0LWxpbmUgQHR5cGVzY3JpcHQtZXNsaW50L25vLW5hbWVzcGFjZVxuZXhwb3J0IG5hbWVzcGFjZSBWUk1TY2hlbWEge1xuICAvKipcbiAgICogVlJNIGV4dGVuc2lvbiBpcyBmb3IgM2QgaHVtYW5vaWQgYXZhdGFycyAoYW5kIG1vZGVscykgaW4gVlIgYXBwbGljYXRpb25zLlxuICAgKi9cbiAgZXhwb3J0IGludGVyZmFjZSBWUk0ge1xuICAgIGJsZW5kU2hhcGVNYXN0ZXI/OiBCbGVuZFNoYXBlO1xuICAgIC8qKlxuICAgICAqIFZlcnNpb24gb2YgZXhwb3J0ZXIgdGhhdCB2cm0gY3JlYXRlZC4gVW5pVlJNLTAuNTMuMFxuICAgICAqL1xuICAgIGV4cG9ydGVyVmVyc2lvbj86IHN0cmluZztcbiAgICBmaXJzdFBlcnNvbj86IEZpcnN0UGVyc29uO1xuICAgIGh1bWFub2lkPzogSHVtYW5vaWQ7XG4gICAgbWF0ZXJpYWxQcm9wZXJ0aWVzPzogTWF0ZXJpYWxbXTtcbiAgICBtZXRhPzogTWV0YTtcbiAgICBzZWNvbmRhcnlBbmltYXRpb24/OiBTZWNvbmRhcnlBbmltYXRpb247XG4gICAgLyoqXG4gICAgICogVmVyc2lvbiBvZiBWUk0gc3BlY2lmaWNhdGlvbi4gMC4wXG4gICAgICovXG4gICAgc3BlY1ZlcnNpb24/OiBzdHJpbmc7XG4gIH1cblxuICAvKipcbiAgICogQmxlbmRTaGFwZUF2YXRhciBvZiBVbmlWUk1cbiAgICovXG4gIGV4cG9ydCBpbnRlcmZhY2UgQmxlbmRTaGFwZSB7XG4gICAgYmxlbmRTaGFwZUdyb3Vwcz86IEJsZW5kU2hhcGVHcm91cFtdO1xuICB9XG5cbiAgZXhwb3J0IGludGVyZmFjZSBCbGVuZFNoYXBlR3JvdXAge1xuICAgIC8qKlxuICAgICAqIExvdyBsZXZlbCBibGVuZHNoYXBlIHJlZmVyZW5jZXMuXG4gICAgICovXG4gICAgYmluZHM/OiBCbGVuZFNoYXBlQmluZFtdO1xuICAgIC8qKlxuICAgICAqIDAgb3IgMS4gRG8gbm90IGFsbG93IGFuIGludGVybWVkaWF0ZSB2YWx1ZS4gVmFsdWUgc2hvdWxkIHJvdW5kZWRcbiAgICAgKi9cbiAgICBpc0JpbmFyeT86IGJvb2xlYW47XG4gICAgLyoqXG4gICAgICogTWF0ZXJpYWwgYW5pbWF0aW9uIHJlZmVyZW5jZXMuXG4gICAgICovXG4gICAgbWF0ZXJpYWxWYWx1ZXM/OiBCbGVuZFNoYXBlTWF0ZXJpYWxiaW5kW107XG4gICAgLyoqXG4gICAgICogRXhwcmVzc2lvbiBuYW1lXG4gICAgICovXG4gICAgbmFtZT86IHN0cmluZztcbiAgICAvKipcbiAgICAgKiBQcmVkZWZpbmVkIEV4cHJlc3Npb24gbmFtZVxuICAgICAqL1xuICAgIHByZXNldE5hbWU/OiBCbGVuZFNoYXBlUHJlc2V0TmFtZTtcbiAgfVxuXG4gIGV4cG9ydCBpbnRlcmZhY2UgQmxlbmRTaGFwZUJpbmQge1xuICAgIGluZGV4PzogbnVtYmVyO1xuICAgIG1lc2g/OiBudW1iZXI7XG4gICAgLyoqXG4gICAgICogU2tpbm5lZE1lc2hSZW5kZXJlci5TZXRCbGVuZFNoYXBlV2VpZ2h0XG4gICAgICovXG4gICAgd2VpZ2h0PzogbnVtYmVyO1xuICB9XG5cbiAgZXhwb3J0IGludGVyZmFjZSBCbGVuZFNoYXBlTWF0ZXJpYWxiaW5kIHtcbiAgICBtYXRlcmlhbE5hbWU/OiBzdHJpbmc7XG4gICAgcHJvcGVydHlOYW1lPzogc3RyaW5nO1xuICAgIHRhcmdldFZhbHVlPzogbnVtYmVyW107XG4gIH1cblxuICAvKipcbiAgICogUHJlZGVmaW5lZCBFeHByZXNzaW9uIG5hbWVcbiAgICovXG4gIGV4cG9ydCBlbnVtIEJsZW5kU2hhcGVQcmVzZXROYW1lIHtcbiAgICBBID0gJ2EnLFxuICAgIEFuZ3J5ID0gJ2FuZ3J5JyxcbiAgICBCbGluayA9ICdibGluaycsXG4gICAgQmxpbmtMID0gJ2JsaW5rX2wnLFxuICAgIEJsaW5rUiA9ICdibGlua19yJyxcbiAgICBFID0gJ2UnLFxuICAgIEZ1biA9ICdmdW4nLFxuICAgIEkgPSAnaScsXG4gICAgSm95ID0gJ2pveScsXG4gICAgTG9va2Rvd24gPSAnbG9va2Rvd24nLFxuICAgIExvb2tsZWZ0ID0gJ2xvb2tsZWZ0JyxcbiAgICBMb29rcmlnaHQgPSAnbG9va3JpZ2h0JyxcbiAgICBMb29rdXAgPSAnbG9va3VwJyxcbiAgICBOZXV0cmFsID0gJ25ldXRyYWwnLFxuICAgIE8gPSAnbycsXG4gICAgU29ycm93ID0gJ3NvcnJvdycsXG4gICAgVSA9ICd1JyxcbiAgICBVbmtub3duID0gJ3Vua25vd24nLFxuICB9XG5cbiAgZXhwb3J0IGludGVyZmFjZSBGaXJzdFBlcnNvbiB7XG4gICAgLyoqXG4gICAgICogVGhlIGJvbmUgd2hvc2UgcmVuZGVyaW5nIHNob3VsZCBiZSB0dXJuZWQgb2ZmIGluIGZpcnN0LXBlcnNvbiB2aWV3LiBVc3VhbGx5IEhlYWQgaXNcbiAgICAgKiBzcGVjaWZpZWQuXG4gICAgICovXG4gICAgZmlyc3RQZXJzb25Cb25lPzogbnVtYmVyO1xuICAgIC8qKlxuICAgICAqIFRoZSB0YXJnZXQgcG9zaXRpb24gb2YgdGhlIFZSIGhlYWRzZXQgaW4gZmlyc3QtcGVyc29uIHZpZXcuIEl0IGlzIGFzc3VtZWQgdGhhdCBhbiBvZmZzZXRcbiAgICAgKiBmcm9tIHRoZSBoZWFkIGJvbmUgdG8gdGhlIFZSIGhlYWRzZXQgaXMgYWRkZWQuXG4gICAgICovXG4gICAgZmlyc3RQZXJzb25Cb25lT2Zmc2V0PzogVmVjdG9yMztcbiAgICBsb29rQXRIb3Jpem9udGFsSW5uZXI/OiBGaXJzdFBlcnNvbkRlZ3JlZU1hcDtcbiAgICBsb29rQXRIb3Jpem9udGFsT3V0ZXI/OiBGaXJzdFBlcnNvbkRlZ3JlZU1hcDtcbiAgICAvKipcbiAgICAgKiBFeWUgY29udHJvbGxlciBtb2RlLlxuICAgICAqL1xuICAgIGxvb2tBdFR5cGVOYW1lPzogRmlyc3RQZXJzb25Mb29rQXRUeXBlTmFtZTtcbiAgICBsb29rQXRWZXJ0aWNhbERvd24/OiBGaXJzdFBlcnNvbkRlZ3JlZU1hcDtcbiAgICBsb29rQXRWZXJ0aWNhbFVwPzogRmlyc3RQZXJzb25EZWdyZWVNYXA7XG4gICAgLyoqXG4gICAgICogU3dpdGNoIGRpc3BsYXkgLyB1bmRpc3BsYXkgZm9yIGVhY2ggbWVzaCBpbiBmaXJzdC1wZXJzb24gdmlldyBvciB0aGUgb3RoZXJzLlxuICAgICAqL1xuICAgIG1lc2hBbm5vdGF0aW9ucz86IEZpcnN0UGVyc29uTWVzaGFubm90YXRpb25bXTtcbiAgfVxuXG4gIC8qKlxuICAgKiBFeWUgY29udHJvbGxlciBzZXR0aW5nLlxuICAgKi9cbiAgZXhwb3J0IGludGVyZmFjZSBGaXJzdFBlcnNvbkRlZ3JlZU1hcCB7XG4gICAgLyoqXG4gICAgICogTm9uZSBsaW5lYXIgbWFwcGluZyBwYXJhbXMuIHRpbWUsIHZhbHVlLCBpblRhbmdlbnQsIG91dFRhbmdlbnRcbiAgICAgKi9cbiAgICBjdXJ2ZT86IG51bWJlcltdO1xuICAgIC8qKlxuICAgICAqIExvb2sgYXQgaW5wdXQgY2xhbXAgcmFuZ2UgZGVncmVlLlxuICAgICAqL1xuICAgIHhSYW5nZT86IG51bWJlcjtcbiAgICAvKipcbiAgICAgKiBMb29rIGF0IG1hcCByYW5nZSBkZWdyZWUgZnJvbSB4UmFuZ2UuXG4gICAgICovXG4gICAgeVJhbmdlPzogbnVtYmVyO1xuICB9XG5cbiAgLyoqXG4gICAqIEV5ZSBjb250cm9sbGVyIG1vZGUuXG4gICAqL1xuICBleHBvcnQgZW51bSBGaXJzdFBlcnNvbkxvb2tBdFR5cGVOYW1lIHtcbiAgICBCbGVuZFNoYXBlID0gJ0JsZW5kU2hhcGUnLFxuICAgIEJvbmUgPSAnQm9uZScsXG4gIH1cblxuICBleHBvcnQgaW50ZXJmYWNlIEZpcnN0UGVyc29uTWVzaGFubm90YXRpb24ge1xuICAgIGZpcnN0UGVyc29uRmxhZz86IHN0cmluZztcbiAgICBtZXNoPzogbnVtYmVyO1xuICB9XG5cbiAgZXhwb3J0IGludGVyZmFjZSBIdW1hbm9pZCB7XG4gICAgLyoqXG4gICAgICogVW5pdHkncyBIdW1hbkRlc2NyaXB0aW9uLmFybVN0cmV0Y2hcbiAgICAgKi9cbiAgICBhcm1TdHJldGNoPzogbnVtYmVyO1xuICAgIC8qKlxuICAgICAqIFVuaXR5J3MgSHVtYW5EZXNjcmlwdGlvbi5mZWV0U3BhY2luZ1xuICAgICAqL1xuICAgIGZlZXRTcGFjaW5nPzogbnVtYmVyO1xuICAgIC8qKlxuICAgICAqIFVuaXR5J3MgSHVtYW5EZXNjcmlwdGlvbi5oYXNUcmFuc2xhdGlvbkRvRlxuICAgICAqL1xuICAgIGhhc1RyYW5zbGF0aW9uRG9GPzogYm9vbGVhbjtcbiAgICBodW1hbkJvbmVzPzogSHVtYW5vaWRCb25lW107XG4gICAgLyoqXG4gICAgICogVW5pdHkncyBIdW1hbkRlc2NyaXB0aW9uLmxlZ1N0cmV0Y2hcbiAgICAgKi9cbiAgICBsZWdTdHJldGNoPzogbnVtYmVyO1xuICAgIC8qKlxuICAgICAqIFVuaXR5J3MgSHVtYW5EZXNjcmlwdGlvbi5sb3dlckFybVR3aXN0XG4gICAgICovXG4gICAgbG93ZXJBcm1Ud2lzdD86IG51bWJlcjtcbiAgICAvKipcbiAgICAgKiBVbml0eSdzIEh1bWFuRGVzY3JpcHRpb24ubG93ZXJMZWdUd2lzdFxuICAgICAqL1xuICAgIGxvd2VyTGVnVHdpc3Q/OiBudW1iZXI7XG4gICAgLyoqXG4gICAgICogVW5pdHkncyBIdW1hbkRlc2NyaXB0aW9uLnVwcGVyQXJtVHdpc3RcbiAgICAgKi9cbiAgICB1cHBlckFybVR3aXN0PzogbnVtYmVyO1xuICAgIC8qKlxuICAgICAqIFVuaXR5J3MgSHVtYW5EZXNjcmlwdGlvbi51cHBlckxlZ1R3aXN0XG4gICAgICovXG4gICAgdXBwZXJMZWdUd2lzdD86IG51bWJlcjtcbiAgfVxuXG4gIGV4cG9ydCBpbnRlcmZhY2UgSHVtYW5vaWRCb25lIHtcbiAgICAvKipcbiAgICAgKiBVbml0eSdzIEh1bWFuTGltaXQuYXhpc0xlbmd0aFxuICAgICAqL1xuICAgIGF4aXNMZW5ndGg/OiBudW1iZXI7XG4gICAgLyoqXG4gICAgICogSHVtYW4gYm9uZSBuYW1lLlxuICAgICAqL1xuICAgIGJvbmU/OiBIdW1hbm9pZEJvbmVOYW1lO1xuICAgIC8qKlxuICAgICAqIFVuaXR5J3MgSHVtYW5MaW1pdC5jZW50ZXJcbiAgICAgKi9cbiAgICBjZW50ZXI/OiBWZWN0b3IzO1xuICAgIC8qKlxuICAgICAqIFVuaXR5J3MgSHVtYW5MaW1pdC5tYXhcbiAgICAgKi9cbiAgICBtYXg/OiBWZWN0b3IzO1xuICAgIC8qKlxuICAgICAqIFVuaXR5J3MgSHVtYW5MaW1pdC5taW5cbiAgICAgKi9cbiAgICBtaW4/OiBWZWN0b3IzO1xuICAgIC8qKlxuICAgICAqIFJlZmVyZW5jZSBub2RlIGluZGV4XG4gICAgICovXG4gICAgbm9kZT86IG51bWJlcjtcbiAgICAvKipcbiAgICAgKiBVbml0eSdzIEh1bWFuTGltaXQudXNlRGVmYXVsdFZhbHVlc1xuICAgICAqL1xuICAgIHVzZURlZmF1bHRWYWx1ZXM/OiBib29sZWFuO1xuICB9XG5cbiAgLyoqXG4gICAqIEh1bWFuIGJvbmUgbmFtZS5cbiAgICovXG4gIGV4cG9ydCBlbnVtIEh1bWFub2lkQm9uZU5hbWUge1xuICAgIENoZXN0ID0gJ2NoZXN0JyxcbiAgICBIZWFkID0gJ2hlYWQnLFxuICAgIEhpcHMgPSAnaGlwcycsXG4gICAgSmF3ID0gJ2phdycsXG4gICAgTGVmdEV5ZSA9ICdsZWZ0RXllJyxcbiAgICBMZWZ0Rm9vdCA9ICdsZWZ0Rm9vdCcsXG4gICAgTGVmdEhhbmQgPSAnbGVmdEhhbmQnLFxuICAgIExlZnRJbmRleERpc3RhbCA9ICdsZWZ0SW5kZXhEaXN0YWwnLFxuICAgIExlZnRJbmRleEludGVybWVkaWF0ZSA9ICdsZWZ0SW5kZXhJbnRlcm1lZGlhdGUnLFxuICAgIExlZnRJbmRleFByb3hpbWFsID0gJ2xlZnRJbmRleFByb3hpbWFsJyxcbiAgICBMZWZ0TGl0dGxlRGlzdGFsID0gJ2xlZnRMaXR0bGVEaXN0YWwnLFxuICAgIExlZnRMaXR0bGVJbnRlcm1lZGlhdGUgPSAnbGVmdExpdHRsZUludGVybWVkaWF0ZScsXG4gICAgTGVmdExpdHRsZVByb3hpbWFsID0gJ2xlZnRMaXR0bGVQcm94aW1hbCcsXG4gICAgTGVmdExvd2VyQXJtID0gJ2xlZnRMb3dlckFybScsXG4gICAgTGVmdExvd2VyTGVnID0gJ2xlZnRMb3dlckxlZycsXG4gICAgTGVmdE1pZGRsZURpc3RhbCA9ICdsZWZ0TWlkZGxlRGlzdGFsJyxcbiAgICBMZWZ0TWlkZGxlSW50ZXJtZWRpYXRlID0gJ2xlZnRNaWRkbGVJbnRlcm1lZGlhdGUnLFxuICAgIExlZnRNaWRkbGVQcm94aW1hbCA9ICdsZWZ0TWlkZGxlUHJveGltYWwnLFxuICAgIExlZnRSaW5nRGlzdGFsID0gJ2xlZnRSaW5nRGlzdGFsJyxcbiAgICBMZWZ0UmluZ0ludGVybWVkaWF0ZSA9ICdsZWZ0UmluZ0ludGVybWVkaWF0ZScsXG4gICAgTGVmdFJpbmdQcm94aW1hbCA9ICdsZWZ0UmluZ1Byb3hpbWFsJyxcbiAgICBMZWZ0U2hvdWxkZXIgPSAnbGVmdFNob3VsZGVyJyxcbiAgICBMZWZ0VGh1bWJEaXN0YWwgPSAnbGVmdFRodW1iRGlzdGFsJyxcbiAgICBMZWZ0VGh1bWJJbnRlcm1lZGlhdGUgPSAnbGVmdFRodW1iSW50ZXJtZWRpYXRlJyxcbiAgICBMZWZ0VGh1bWJQcm94aW1hbCA9ICdsZWZ0VGh1bWJQcm94aW1hbCcsXG4gICAgTGVmdFRvZXMgPSAnbGVmdFRvZXMnLFxuICAgIExlZnRVcHBlckFybSA9ICdsZWZ0VXBwZXJBcm0nLFxuICAgIExlZnRVcHBlckxlZyA9ICdsZWZ0VXBwZXJMZWcnLFxuICAgIE5lY2sgPSAnbmVjaycsXG4gICAgUmlnaHRFeWUgPSAncmlnaHRFeWUnLFxuICAgIFJpZ2h0Rm9vdCA9ICdyaWdodEZvb3QnLFxuICAgIFJpZ2h0SGFuZCA9ICdyaWdodEhhbmQnLFxuICAgIFJpZ2h0SW5kZXhEaXN0YWwgPSAncmlnaHRJbmRleERpc3RhbCcsXG4gICAgUmlnaHRJbmRleEludGVybWVkaWF0ZSA9ICdyaWdodEluZGV4SW50ZXJtZWRpYXRlJyxcbiAgICBSaWdodEluZGV4UHJveGltYWwgPSAncmlnaHRJbmRleFByb3hpbWFsJyxcbiAgICBSaWdodExpdHRsZURpc3RhbCA9ICdyaWdodExpdHRsZURpc3RhbCcsXG4gICAgUmlnaHRMaXR0bGVJbnRlcm1lZGlhdGUgPSAncmlnaHRMaXR0bGVJbnRlcm1lZGlhdGUnLFxuICAgIFJpZ2h0TGl0dGxlUHJveGltYWwgPSAncmlnaHRMaXR0bGVQcm94aW1hbCcsXG4gICAgUmlnaHRMb3dlckFybSA9ICdyaWdodExvd2VyQXJtJyxcbiAgICBSaWdodExvd2VyTGVnID0gJ3JpZ2h0TG93ZXJMZWcnLFxuICAgIFJpZ2h0TWlkZGxlRGlzdGFsID0gJ3JpZ2h0TWlkZGxlRGlzdGFsJyxcbiAgICBSaWdodE1pZGRsZUludGVybWVkaWF0ZSA9ICdyaWdodE1pZGRsZUludGVybWVkaWF0ZScsXG4gICAgUmlnaHRNaWRkbGVQcm94aW1hbCA9ICdyaWdodE1pZGRsZVByb3hpbWFsJyxcbiAgICBSaWdodFJpbmdEaXN0YWwgPSAncmlnaHRSaW5nRGlzdGFsJyxcbiAgICBSaWdodFJpbmdJbnRlcm1lZGlhdGUgPSAncmlnaHRSaW5nSW50ZXJtZWRpYXRlJyxcbiAgICBSaWdodFJpbmdQcm94aW1hbCA9ICdyaWdodFJpbmdQcm94aW1hbCcsXG4gICAgUmlnaHRTaG91bGRlciA9ICdyaWdodFNob3VsZGVyJyxcbiAgICBSaWdodFRodW1iRGlzdGFsID0gJ3JpZ2h0VGh1bWJEaXN0YWwnLFxuICAgIFJpZ2h0VGh1bWJJbnRlcm1lZGlhdGUgPSAncmlnaHRUaHVtYkludGVybWVkaWF0ZScsXG4gICAgUmlnaHRUaHVtYlByb3hpbWFsID0gJ3JpZ2h0VGh1bWJQcm94aW1hbCcsXG4gICAgUmlnaHRUb2VzID0gJ3JpZ2h0VG9lcycsXG4gICAgUmlnaHRVcHBlckFybSA9ICdyaWdodFVwcGVyQXJtJyxcbiAgICBSaWdodFVwcGVyTGVnID0gJ3JpZ2h0VXBwZXJMZWcnLFxuICAgIFNwaW5lID0gJ3NwaW5lJyxcbiAgICBVcHBlckNoZXN0ID0gJ3VwcGVyQ2hlc3QnLFxuICB9XG5cbiAgZXhwb3J0IGludGVyZmFjZSBNYXRlcmlhbCB7XG4gICAgZmxvYXRQcm9wZXJ0aWVzPzogeyBba2V5OiBzdHJpbmddOiBhbnkgfTtcbiAgICBrZXl3b3JkTWFwPzogeyBba2V5OiBzdHJpbmddOiBhbnkgfTtcbiAgICBuYW1lPzogc3RyaW5nO1xuICAgIHJlbmRlclF1ZXVlPzogbnVtYmVyO1xuICAgIHNoYWRlcj86IHN0cmluZztcbiAgICB0YWdNYXA/OiB7IFtrZXk6IHN0cmluZ106IGFueSB9O1xuICAgIHRleHR1cmVQcm9wZXJ0aWVzPzogeyBba2V5OiBzdHJpbmddOiBhbnkgfTtcbiAgICB2ZWN0b3JQcm9wZXJ0aWVzPzogeyBba2V5OiBzdHJpbmddOiBhbnkgfTtcbiAgfVxuXG4gIGV4cG9ydCBpbnRlcmZhY2UgTWV0YSB7XG4gICAgLyoqXG4gICAgICogQSBwZXJzb24gd2hvIGNhbiBwZXJmb3JtIHdpdGggdGhpcyBhdmF0YXJcbiAgICAgKi9cbiAgICBhbGxvd2VkVXNlck5hbWU/OiBNZXRhQWxsb3dlZFVzZXJOYW1lO1xuICAgIC8qKlxuICAgICAqIEF1dGhvciBvZiBWUk0gbW9kZWxcbiAgICAgKi9cbiAgICBhdXRob3I/OiBzdHJpbmc7XG4gICAgLyoqXG4gICAgICogRm9yIGNvbW1lcmNpYWwgdXNlXG4gICAgICovXG4gICAgY29tbWVyY2lhbFVzc2FnZU5hbWU/OiBNZXRhVXNzYWdlTmFtZTtcbiAgICAvKipcbiAgICAgKiBDb250YWN0IEluZm9ybWF0aW9uIG9mIFZSTSBtb2RlbCBhdXRob3JcbiAgICAgKi9cbiAgICBjb250YWN0SW5mb3JtYXRpb24/OiBzdHJpbmc7XG4gICAgLyoqXG4gICAgICogTGljZW5zZSB0eXBlXG4gICAgICovXG4gICAgbGljZW5zZU5hbWU/OiBNZXRhTGljZW5zZU5hbWU7XG4gICAgLyoqXG4gICAgICogSWYg4oCcT3RoZXLigJ0gaXMgc2VsZWN0ZWQsIHB1dCB0aGUgVVJMIGxpbmsgb2YgdGhlIGxpY2Vuc2UgZG9jdW1lbnQgaGVyZS5cbiAgICAgKi9cbiAgICBvdGhlckxpY2Vuc2VVcmw/OiBzdHJpbmc7XG4gICAgLyoqXG4gICAgICogSWYgdGhlcmUgYXJlIGFueSBjb25kaXRpb25zIG5vdCBtZW50aW9uZWQgYWJvdmUsIHB1dCB0aGUgVVJMIGxpbmsgb2YgdGhlIGxpY2Vuc2UgZG9jdW1lbnRcbiAgICAgKiBoZXJlLlxuICAgICAqL1xuICAgIG90aGVyUGVybWlzc2lvblVybD86IHN0cmluZztcbiAgICAvKipcbiAgICAgKiBSZWZlcmVuY2Ugb2YgVlJNIG1vZGVsXG4gICAgICovXG4gICAgcmVmZXJlbmNlPzogc3RyaW5nO1xuICAgIC8qKlxuICAgICAqIFBlcm1pc3Npb24gdG8gcGVyZm9ybSBzZXh1YWwgYWN0cyB3aXRoIHRoaXMgYXZhdGFyXG4gICAgICovXG4gICAgc2V4dWFsVXNzYWdlTmFtZT86IE1ldGFVc3NhZ2VOYW1lO1xuICAgIC8qKlxuICAgICAqIFRodW1ibmFpbCBvZiBWUk0gbW9kZWxcbiAgICAgKi9cbiAgICB0ZXh0dXJlPzogbnVtYmVyO1xuICAgIC8qKlxuICAgICAqIFRpdGxlIG9mIFZSTSBtb2RlbFxuICAgICAqL1xuICAgIHRpdGxlPzogc3RyaW5nO1xuICAgIC8qKlxuICAgICAqIFZlcnNpb24gb2YgVlJNIG1vZGVsXG4gICAgICovXG4gICAgdmVyc2lvbj86IHN0cmluZztcbiAgICAvKipcbiAgICAgKiBQZXJtaXNzaW9uIHRvIHBlcmZvcm0gdmlvbGVudCBhY3RzIHdpdGggdGhpcyBhdmF0YXJcbiAgICAgKi9cbiAgICB2aW9sZW50VXNzYWdlTmFtZT86IE1ldGFVc3NhZ2VOYW1lO1xuICB9XG5cbiAgLyoqXG4gICAqIEEgcGVyc29uIHdobyBjYW4gcGVyZm9ybSB3aXRoIHRoaXMgYXZhdGFyXG4gICAqL1xuICBleHBvcnQgZW51bSBNZXRhQWxsb3dlZFVzZXJOYW1lIHtcbiAgICBFdmVyeW9uZSA9ICdFdmVyeW9uZScsXG4gICAgRXhwbGljaXRseUxpY2Vuc2VkUGVyc29uID0gJ0V4cGxpY2l0bHlMaWNlbnNlZFBlcnNvbicsXG4gICAgT25seUF1dGhvciA9ICdPbmx5QXV0aG9yJyxcbiAgfVxuXG4gIC8qKlxuICAgKiBGb3IgY29tbWVyY2lhbCB1c2VcbiAgICpcbiAgICogUGVybWlzc2lvbiB0byBwZXJmb3JtIHNleHVhbCBhY3RzIHdpdGggdGhpcyBhdmF0YXJcbiAgICpcbiAgICogUGVybWlzc2lvbiB0byBwZXJmb3JtIHZpb2xlbnQgYWN0cyB3aXRoIHRoaXMgYXZhdGFyXG4gICAqL1xuICBleHBvcnQgZW51bSBNZXRhVXNzYWdlTmFtZSB7XG4gICAgQWxsb3cgPSAnQWxsb3cnLFxuICAgIERpc2FsbG93ID0gJ0Rpc2FsbG93JyxcbiAgfVxuXG4gIC8qKlxuICAgKiBMaWNlbnNlIHR5cGVcbiAgICovXG4gIGV4cG9ydCBlbnVtIE1ldGFMaWNlbnNlTmFtZSB7XG4gICAgQ2MwID0gJ0NDMCcsXG4gICAgQ2NCeSA9ICdDQ19CWScsXG4gICAgQ2NCeU5jID0gJ0NDX0JZX05DJyxcbiAgICBDY0J5TmNOZCA9ICdDQ19CWV9OQ19ORCcsXG4gICAgQ2NCeU5jU2EgPSAnQ0NfQllfTkNfU0EnLFxuICAgIENjQnlOZCA9ICdDQ19CWV9ORCcsXG4gICAgQ2NCeVNhID0gJ0NDX0JZX1NBJyxcbiAgICBPdGhlciA9ICdPdGhlcicsXG4gICAgUmVkaXN0cmlidXRpb25Qcm9oaWJpdGVkID0gJ1JlZGlzdHJpYnV0aW9uX1Byb2hpYml0ZWQnLFxuICB9XG5cbiAgLyoqXG4gICAqIFRoZSBzZXR0aW5nIG9mIGF1dG9tYXRpYyBhbmltYXRpb24gb2Ygc3RyaW5nLWxpa2Ugb2JqZWN0cyBzdWNoIGFzIHRhaWxzIGFuZCBoYWlycy5cbiAgICovXG4gIGV4cG9ydCBpbnRlcmZhY2UgU2Vjb25kYXJ5QW5pbWF0aW9uIHtcbiAgICBib25lR3JvdXBzPzogU2Vjb25kYXJ5QW5pbWF0aW9uU3ByaW5nW107XG4gICAgY29sbGlkZXJHcm91cHM/OiBTZWNvbmRhcnlBbmltYXRpb25Db2xsaWRlcmdyb3VwW107XG4gIH1cblxuICBleHBvcnQgaW50ZXJmYWNlIFNlY29uZGFyeUFuaW1hdGlvblNwcmluZyB7XG4gICAgLyoqXG4gICAgICogU3BlY2lmeSB0aGUgbm9kZSBpbmRleCBvZiB0aGUgcm9vdCBib25lIG9mIHRoZSBzd2F5aW5nIG9iamVjdC5cbiAgICAgKi9cbiAgICBib25lcz86IG51bWJlcltdO1xuICAgIC8qKlxuICAgICAqIFRoZSByZWZlcmVuY2UgcG9pbnQgb2YgYSBzd2F5aW5nIG9iamVjdCBjYW4gYmUgc2V0IGF0IGFueSBsb2NhdGlvbiBleGNlcHQgdGhlIG9yaWdpbi5cbiAgICAgKiBXaGVuIGltcGxlbWVudGluZyBVSSBtb3Zpbmcgd2l0aCB3YXJwLCB0aGUgcGFyZW50IG5vZGUgdG8gbW92ZSB3aXRoIHdhcnAgY2FuIGJlIHNwZWNpZmllZFxuICAgICAqIGlmIHlvdSBkb24ndCB3YW50IHRvIG1ha2UgdGhlIG9iamVjdCBzd2F5aW5nIHdpdGggd2FycCBtb3ZlbWVudC5cbiAgICAgKi9cbiAgICBjZW50ZXI/OiBudW1iZXI7XG4gICAgLyoqXG4gICAgICogU3BlY2lmeSB0aGUgaW5kZXggb2YgdGhlIGNvbGxpZGVyIGdyb3VwIGZvciBjb2xsaXNpb25zIHdpdGggc3dheWluZyBvYmplY3RzLlxuICAgICAqL1xuICAgIGNvbGxpZGVyR3JvdXBzPzogbnVtYmVyW107XG4gICAgLyoqXG4gICAgICogQW5ub3RhdGlvbiBjb21tZW50XG4gICAgICovXG4gICAgY29tbWVudD86IHN0cmluZztcbiAgICAvKipcbiAgICAgKiBUaGUgcmVzaXN0YW5jZSAoZGVjZWxlcmF0aW9uKSBvZiBhdXRvbWF0aWMgYW5pbWF0aW9uLlxuICAgICAqL1xuICAgIGRyYWdGb3JjZT86IG51bWJlcjtcbiAgICAvKipcbiAgICAgKiBUaGUgZGlyZWN0aW9uIG9mIGdyYXZpdHkuIFNldCAoMCwgLTEsIDApIGZvciBzaW11bGF0aW5nIHRoZSBncmF2aXR5LiBTZXQgKDEsIDAsIDApIGZvclxuICAgICAqIHNpbXVsYXRpbmcgdGhlIHdpbmQuXG4gICAgICovXG4gICAgZ3Jhdml0eURpcj86IFZlY3RvcjM7XG4gICAgLyoqXG4gICAgICogVGhlIHN0cmVuZ3RoIG9mIGdyYXZpdHkuXG4gICAgICovXG4gICAgZ3Jhdml0eVBvd2VyPzogbnVtYmVyO1xuICAgIC8qKlxuICAgICAqIFRoZSByYWRpdXMgb2YgdGhlIHNwaGVyZSB1c2VkIGZvciB0aGUgY29sbGlzaW9uIGRldGVjdGlvbiB3aXRoIGNvbGxpZGVycy5cbiAgICAgKi9cbiAgICBoaXRSYWRpdXM/OiBudW1iZXI7XG4gICAgLyoqXG4gICAgICogVGhlIHJlc2lsaWVuY2Ugb2YgdGhlIHN3YXlpbmcgb2JqZWN0ICh0aGUgcG93ZXIgb2YgcmV0dXJuaW5nIHRvIHRoZSBpbml0aWFsIHBvc2UpLlxuICAgICAqL1xuICAgIHN0aWZmaW5lc3M/OiBudW1iZXI7XG4gIH1cblxuICBleHBvcnQgaW50ZXJmYWNlIFNlY29uZGFyeUFuaW1hdGlvbkNvbGxpZGVyZ3JvdXAge1xuICAgIGNvbGxpZGVycz86IFNlY29uZGFyeUFuaW1hdGlvbkNvbGxpZGVyW107XG4gICAgLyoqXG4gICAgICogVGhlIG5vZGUgb2YgdGhlIGNvbGxpZGVyIGdyb3VwIGZvciBzZXR0aW5nIHVwIGNvbGxpc2lvbiBkZXRlY3Rpb25zLlxuICAgICAqL1xuICAgIG5vZGU/OiBudW1iZXI7XG4gIH1cblxuICBleHBvcnQgaW50ZXJmYWNlIFNlY29uZGFyeUFuaW1hdGlvbkNvbGxpZGVyIHtcbiAgICAvKipcbiAgICAgKiBUaGUgbG9jYWwgY29vcmRpbmF0ZSBmcm9tIHRoZSBub2RlIG9mIHRoZSBjb2xsaWRlciBncm91cC5cbiAgICAgKi9cbiAgICBvZmZzZXQ/OiBWZWN0b3IzO1xuICAgIC8qKlxuICAgICAqIFRoZSByYWRpdXMgb2YgdGhlIGNvbGxpZGVyLlxuICAgICAqL1xuICAgIHJhZGl1cz86IG51bWJlcjtcbiAgfVxuXG4gIGV4cG9ydCBpbnRlcmZhY2UgVmVjdG9yMyB7XG4gICAgeD86IG51bWJlcjtcbiAgICB5PzogbnVtYmVyO1xuICAgIHo/OiBudW1iZXI7XG4gIH1cbn1cbiIsImltcG9ydCB0eXBlIHsgR0xURiB9IGZyb20gJ3RocmVlL2V4YW1wbGVzL2pzbS9sb2FkZXJzL0dMVEZMb2FkZXInO1xuaW1wb3J0IHR5cGUgeyBHTFRGUHJpbWl0aXZlLCBHTFRGU2NoZW1hIH0gZnJvbSAnLi4vdHlwZXMnO1xuXG5mdW5jdGlvbiBleHRyYWN0UHJpbWl0aXZlc0ludGVybmFsKGdsdGY6IEdMVEYsIG5vZGVJbmRleDogbnVtYmVyLCBub2RlOiBUSFJFRS5PYmplY3QzRCk6IEdMVEZQcmltaXRpdmVbXSB8IG51bGwge1xuICAvKipcbiAgICogTGV0J3MgbGlzdCB1cCBldmVyeSBwb3NzaWJsZSBwYXR0ZXJucyB0aGF0IHBhcnNlZCBnbHRmIG5vZGVzIHdpdGggYSBtZXNoIGNhbiBoYXZlLCwsXG4gICAqXG4gICAqIFwiKlwiIGluZGljYXRlcyB0aGF0IHRob3NlIG1lc2hlcyBzaG91bGQgYmUgbGlzdGVkIHVwIHVzaW5nIHRoaXMgZnVuY3Rpb25cbiAgICpcbiAgICogIyMjIEEgbm9kZSB3aXRoIGEgKG1lc2gsIGEgc2lnbmxlIHByaW1pdGl2ZSlcbiAgICpcbiAgICogLSBgVEhSRUUuTWVzaGA6IFRoZSBvbmx5IHByaW1pdGl2ZSBvZiB0aGUgbWVzaCAqXG4gICAqXG4gICAqICMjIyBBIG5vZGUgd2l0aCBhIChtZXNoLCBtdWx0aXBsZSBwcmltaXRpdmVzKVxuICAgKlxuICAgKiAtIGBUSFJFRS5Hcm91cGA6IFRoZSByb290IG9mIHRoZSBtZXNoXG4gICAqICAgLSBgVEhSRUUuTWVzaGA6IEEgcHJpbWl0aXZlIG9mIHRoZSBtZXNoICpcbiAgICogICAtIGBUSFJFRS5NZXNoYDogQSBwcmltaXRpdmUgb2YgdGhlIG1lc2ggKDIpICpcbiAgICpcbiAgICogIyMjIEEgbm9kZSB3aXRoIGEgKG1lc2gsIG11bHRpcGxlIHByaW1pdGl2ZXMpIEFORCAoYSBjaGlsZCB3aXRoIGEgbWVzaCwgYSBzaW5nbGUgcHJpbWl0aXZlKVxuICAgKlxuICAgKiAtIGBUSFJFRS5Hcm91cGA6IFRoZSByb290IG9mIHRoZSBtZXNoXG4gICAqICAgLSBgVEhSRUUuTWVzaGA6IEEgcHJpbWl0aXZlIG9mIHRoZSBtZXNoICpcbiAgICogICAtIGBUSFJFRS5NZXNoYDogQSBwcmltaXRpdmUgb2YgdGhlIG1lc2ggKDIpICpcbiAgICogICAtIGBUSFJFRS5NZXNoYDogQSBwcmltaXRpdmUgb2YgYSBNRVNIIE9GIFRIRSBDSElMRFxuICAgKlxuICAgKiAjIyMgQSBub2RlIHdpdGggYSAobWVzaCwgbXVsdGlwbGUgcHJpbWl0aXZlcykgQU5EIChhIGNoaWxkIHdpdGggYSBtZXNoLCBtdWx0aXBsZSBwcmltaXRpdmVzKVxuICAgKlxuICAgKiAtIGBUSFJFRS5Hcm91cGA6IFRoZSByb290IG9mIHRoZSBtZXNoXG4gICAqICAgLSBgVEhSRUUuTWVzaGA6IEEgcHJpbWl0aXZlIG9mIHRoZSBtZXNoICpcbiAgICogICAtIGBUSFJFRS5NZXNoYDogQSBwcmltaXRpdmUgb2YgdGhlIG1lc2ggKDIpICpcbiAgICogICAtIGBUSFJFRS5Hcm91cGA6IFRoZSByb290IG9mIGEgTUVTSCBPRiBUSEUgQ0hJTERcbiAgICogICAgIC0gYFRIUkVFLk1lc2hgOiBBIHByaW1pdGl2ZSBvZiB0aGUgbWVzaCBvZiB0aGUgY2hpbGRcbiAgICogICAgIC0gYFRIUkVFLk1lc2hgOiBBIHByaW1pdGl2ZSBvZiB0aGUgbWVzaCBvZiB0aGUgY2hpbGQgKDIpXG4gICAqXG4gICAqICMjIyBBIG5vZGUgd2l0aCBhIChtZXNoLCBtdWx0aXBsZSBwcmltaXRpdmVzKSBCVVQgdGhlIG5vZGUgaXMgYSBib25lXG4gICAqXG4gICAqIC0gYFRIUkVFLkJvbmVgOiBUaGUgcm9vdCBvZiB0aGUgbm9kZSwgYXMgYSBib25lXG4gICAqICAgLSBgVEhSRUUuR3JvdXBgOiBUaGUgcm9vdCBvZiB0aGUgbWVzaFxuICAgKiAgICAgLSBgVEhSRUUuTWVzaGA6IEEgcHJpbWl0aXZlIG9mIHRoZSBtZXNoICpcbiAgICogICAgIC0gYFRIUkVFLk1lc2hgOiBBIHByaW1pdGl2ZSBvZiB0aGUgbWVzaCAoMikgKlxuICAgKlxuICAgKiAjIyMgQSBub2RlIHdpdGggYSAobWVzaCwgbXVsdGlwbGUgcHJpbWl0aXZlcykgQU5EIChhIGNoaWxkIHdpdGggYSBtZXNoLCBtdWx0aXBsZSBwcmltaXRpdmVzKSBCVVQgdGhlIG5vZGUgaXMgYSBib25lXG4gICAqXG4gICAqIC0gYFRIUkVFLkJvbmVgOiBUaGUgcm9vdCBvZiB0aGUgbm9kZSwgYXMgYSBib25lXG4gICAqICAgLSBgVEhSRUUuR3JvdXBgOiBUaGUgcm9vdCBvZiB0aGUgbWVzaFxuICAgKiAgICAgLSBgVEhSRUUuTWVzaGA6IEEgcHJpbWl0aXZlIG9mIHRoZSBtZXNoICpcbiAgICogICAgIC0gYFRIUkVFLk1lc2hgOiBBIHByaW1pdGl2ZSBvZiB0aGUgbWVzaCAoMikgKlxuICAgKiAgIC0gYFRIUkVFLkdyb3VwYDogVGhlIHJvb3Qgb2YgYSBNRVNIIE9GIFRIRSBDSElMRFxuICAgKiAgICAgLSBgVEhSRUUuTWVzaGA6IEEgcHJpbWl0aXZlIG9mIHRoZSBtZXNoIG9mIHRoZSBjaGlsZFxuICAgKiAgICAgLSBgVEhSRUUuTWVzaGA6IEEgcHJpbWl0aXZlIG9mIHRoZSBtZXNoIG9mIHRoZSBjaGlsZCAoMilcbiAgICpcbiAgICogLi4uSSB3aWxsIHRha2UgYSBzdHJhdGVneSB0aGF0IHRyYXZlcnNlcyB0aGUgcm9vdCBvZiB0aGUgbm9kZSBhbmQgdGFrZSBmaXJzdCAocHJpbWl0aXZlQ291bnQpIG1lc2hlcy5cbiAgICovXG5cbiAgLy8gTWFrZSBzdXJlIHRoYXQgdGhlIG5vZGUgaGFzIGEgbWVzaFxuICBjb25zdCBzY2hlbWFOb2RlOiBHTFRGU2NoZW1hLk5vZGUgPSBnbHRmLnBhcnNlci5qc29uLm5vZGVzW25vZGVJbmRleF07XG4gIGNvbnN0IG1lc2hJbmRleCA9IHNjaGVtYU5vZGUubWVzaDtcbiAgaWYgKG1lc2hJbmRleCA9PSBudWxsKSB7XG4gICAgcmV0dXJuIG51bGw7XG4gIH1cblxuICAvLyBIb3cgbWFueSBwcmltaXRpdmVzIHRoZSBtZXNoIGhhcz9cbiAgY29uc3Qgc2NoZW1hTWVzaDogR0xURlNjaGVtYS5NZXNoID0gZ2x0Zi5wYXJzZXIuanNvbi5tZXNoZXNbbWVzaEluZGV4XTtcbiAgY29uc3QgcHJpbWl0aXZlQ291bnQgPSBzY2hlbWFNZXNoLnByaW1pdGl2ZXMubGVuZ3RoO1xuXG4gIC8vIFRyYXZlcnNlIHRoZSBub2RlIGFuZCB0YWtlIGZpcnN0IChwcmltaXRpdmVDb3VudCkgbWVzaGVzXG4gIGNvbnN0IHByaW1pdGl2ZXM6IEdMVEZQcmltaXRpdmVbXSA9IFtdO1xuICBub2RlLnRyYXZlcnNlKChvYmplY3QpID0+IHtcbiAgICBpZiAocHJpbWl0aXZlcy5sZW5ndGggPCBwcmltaXRpdmVDb3VudCkge1xuICAgICAgaWYgKChvYmplY3QgYXMgYW55KS5pc01lc2gpIHtcbiAgICAgICAgcHJpbWl0aXZlcy5wdXNoKG9iamVjdCBhcyBHTFRGUHJpbWl0aXZlKTtcbiAgICAgIH1cbiAgICB9XG4gIH0pO1xuXG4gIHJldHVybiBwcmltaXRpdmVzO1xufVxuXG4vKipcbiAqIEV4dHJhY3QgcHJpbWl0aXZlcyAoIGBUSFJFRS5NZXNoW11gICkgb2YgYSBub2RlIGZyb20gYSBsb2FkZWQgR0xURi5cbiAqIFRoZSBtYWluIHB1cnBvc2Ugb2YgdGhpcyBmdW5jdGlvbiBpcyB0byBkaXN0aW5ndWlzaCBwcmltaXRpdmVzIGFuZCBjaGlsZHJlbiBmcm9tIGEgbm9kZSB0aGF0IGhhcyBib3RoIG1lc2hlcyBhbmQgY2hpbGRyZW4uXG4gKlxuICogSXQgdXRpbGl6ZXMgdGhlIGJlaGF2aW9yIHRoYXQgR0xURkxvYWRlciBhZGRzIG1lc2ggcHJpbWl0aXZlcyB0byB0aGUgbm9kZSBvYmplY3QgKCBgVEhSRUUuR3JvdXBgICkgZmlyc3QgdGhlbiBhZGRzIGl0cyBjaGlsZHJlbi5cbiAqXG4gKiBAcGFyYW0gZ2x0ZiBBIEdMVEYgb2JqZWN0IHRha2VuIGZyb20gR0xURkxvYWRlclxuICogQHBhcmFtIG5vZGVJbmRleCBUaGUgaW5kZXggb2YgdGhlIG5vZGVcbiAqL1xuZXhwb3J0IGFzeW5jIGZ1bmN0aW9uIGdsdGZFeHRyYWN0UHJpbWl0aXZlc0Zyb21Ob2RlKGdsdGY6IEdMVEYsIG5vZGVJbmRleDogbnVtYmVyKTogUHJvbWlzZTxHTFRGUHJpbWl0aXZlW10gfCBudWxsPiB7XG4gIGNvbnN0IG5vZGU6IFRIUkVFLk9iamVjdDNEID0gYXdhaXQgZ2x0Zi5wYXJzZXIuZ2V0RGVwZW5kZW5jeSgnbm9kZScsIG5vZGVJbmRleCk7XG4gIHJldHVybiBleHRyYWN0UHJpbWl0aXZlc0ludGVybmFsKGdsdGYsIG5vZGVJbmRleCwgbm9kZSk7XG59XG5cbi8qKlxuICogRXh0cmFjdCBwcmltaXRpdmVzICggYFRIUkVFLk1lc2hbXWAgKSBvZiBub2RlcyBmcm9tIGEgbG9hZGVkIEdMVEYuXG4gKiBTZWUge0BsaW5rIGdsdGZFeHRyYWN0UHJpbWl0aXZlc0Zyb21Ob2RlfSBmb3IgbW9yZSBkZXRhaWxzLlxuICpcbiAqIEl0IHJldHVybnMgYSBtYXAgZnJvbSBub2RlIGluZGV4IHRvIGV4dHJhY3Rpb24gcmVzdWx0LlxuICogSWYgYSBub2RlIGRvZXMgbm90IGhhdmUgYSBtZXNoLCB0aGUgZW50cnkgZm9yIHRoZSBub2RlIHdpbGwgbm90IGJlIHB1dCBpbiB0aGUgcmV0dXJuaW5nIG1hcC5cbiAqXG4gKiBAcGFyYW0gZ2x0ZiBBIEdMVEYgb2JqZWN0IHRha2VuIGZyb20gR0xURkxvYWRlclxuICovXG5leHBvcnQgYXN5bmMgZnVuY3Rpb24gZ2x0ZkV4dHJhY3RQcmltaXRpdmVzRnJvbU5vZGVzKGdsdGY6IEdMVEYpOiBQcm9taXNlPE1hcDxudW1iZXIsIEdMVEZQcmltaXRpdmVbXT4+IHtcbiAgY29uc3Qgbm9kZXM6IFRIUkVFLk9iamVjdDNEW10gPSBhd2FpdCBnbHRmLnBhcnNlci5nZXREZXBlbmRlbmNpZXMoJ25vZGUnKTtcbiAgY29uc3QgbWFwID0gbmV3IE1hcDxudW1iZXIsIEdMVEZQcmltaXRpdmVbXT4oKTtcblxuICBub2Rlcy5mb3JFYWNoKChub2RlLCBpbmRleCkgPT4ge1xuICAgIGNvbnN0IHJlc3VsdCA9IGV4dHJhY3RQcmltaXRpdmVzSW50ZXJuYWwoZ2x0ZiwgaW5kZXgsIG5vZGUpO1xuICAgIGlmIChyZXN1bHQgIT0gbnVsbCkge1xuICAgICAgbWFwLnNldChpbmRleCwgcmVzdWx0KTtcbiAgICB9XG4gIH0pO1xuXG4gIHJldHVybiBtYXA7XG59XG4iLCJleHBvcnQgZnVuY3Rpb24gcmVuYW1lTWF0ZXJpYWxQcm9wZXJ0eShuYW1lOiBzdHJpbmcpOiBzdHJpbmcge1xuICBpZiAobmFtZVswXSAhPT0gJ18nKSB7XG4gICAgY29uc29sZS53YXJuKGByZW5hbWVNYXRlcmlhbFByb3BlcnR5OiBHaXZlbiBwcm9wZXJ0eSBuYW1lIFwiJHtuYW1lfVwiIG1pZ2h0IGJlIGludmFsaWRgKTtcbiAgICByZXR1cm4gbmFtZTtcbiAgfVxuICBuYW1lID0gbmFtZS5zdWJzdHJpbmcoMSk7XG5cbiAgaWYgKCEvW0EtWl0vLnRlc3QobmFtZVswXSkpIHtcbiAgICBjb25zb2xlLndhcm4oYHJlbmFtZU1hdGVyaWFsUHJvcGVydHk6IEdpdmVuIHByb3BlcnR5IG5hbWUgXCIke25hbWV9XCIgbWlnaHQgYmUgaW52YWxpZGApO1xuICAgIHJldHVybiBuYW1lO1xuICB9XG4gIHJldHVybiBuYW1lWzBdLnRvTG93ZXJDYXNlKCkgKyBuYW1lLnN1YnN0cmluZygxKTtcbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcblxuLyoqXG4gKiBDbGFtcCBhbiBpbnB1dCBudW1iZXIgd2l0aGluIFsgYDAuMGAgLSBgMS4wYCBdLlxuICpcbiAqIEBwYXJhbSB2YWx1ZSBUaGUgaW5wdXQgdmFsdWVcbiAqL1xuZXhwb3J0IGZ1bmN0aW9uIHNhdHVyYXRlKHZhbHVlOiBudW1iZXIpOiBudW1iZXIge1xuICByZXR1cm4gTWF0aC5tYXgoTWF0aC5taW4odmFsdWUsIDEuMCksIDAuMCk7XG59XG5cbi8qKlxuICogTWFwIHRoZSByYW5nZSBvZiBhbiBpbnB1dCB2YWx1ZSBmcm9tIFsgYG1pbmAgLSBgbWF4YCBdIHRvIFsgYDAuMGAgLSBgMS4wYCBdLlxuICogSWYgaW5wdXQgdmFsdWUgaXMgbGVzcyB0aGFuIGBtaW5gICwgaXQgcmV0dXJucyBgMC4wYC5cbiAqIElmIGlucHV0IHZhbHVlIGlzIGdyZWF0ZXIgdGhhbiBgbWF4YCAsIGl0IHJldHVybnMgYDEuMGAuXG4gKlxuICogU2VlIGFsc286IGh0dHBzOi8vdGhyZWVqcy5vcmcvZG9jcy8jYXBpL2VuL21hdGgvTWF0aC5zbW9vdGhzdGVwXG4gKlxuICogQHBhcmFtIHggVGhlIHZhbHVlIHRoYXQgd2lsbCBiZSBtYXBwZWQgaW50byB0aGUgc3BlY2lmaWVkIHJhbmdlXG4gKiBAcGFyYW0gbWluIE1pbmltdW0gdmFsdWUgb2YgdGhlIHJhbmdlXG4gKiBAcGFyYW0gbWF4IE1heGltdW0gdmFsdWUgb2YgdGhlIHJhbmdlXG4gKi9cbmV4cG9ydCBmdW5jdGlvbiBsaW5zdGVwKHg6IG51bWJlciwgbWluOiBudW1iZXIsIG1heDogbnVtYmVyKTogbnVtYmVyIHtcbiAgaWYgKHggPD0gbWluKSByZXR1cm4gMDtcbiAgaWYgKHggPj0gbWF4KSByZXR1cm4gMTtcblxuICByZXR1cm4gKHggLSBtaW4pIC8gKG1heCAtIG1pbik7XG59XG5cbmNvbnN0IF9wb3NpdGlvbiA9IG5ldyBUSFJFRS5WZWN0b3IzKCk7XG5jb25zdCBfc2NhbGUgPSBuZXcgVEhSRUUuVmVjdG9yMygpO1xuY29uc3QgX3JvdGF0aW9uID0gbmV3IFRIUkVFLlF1YXRlcm5pb24oKTtcblxuLyoqXG4gKiBFeHRyYWN0IHdvcmxkIHBvc2l0aW9uIG9mIGFuIG9iamVjdCBmcm9tIGl0cyB3b3JsZCBzcGFjZSBtYXRyaXgsIGluIGNoZWFwZXIgd2F5LlxuICpcbiAqIEBwYXJhbSBvYmplY3QgVGhlIG9iamVjdFxuICogQHBhcmFtIG91dCBUYXJnZXQgdmVjdG9yXG4gKi9cbmV4cG9ydCBmdW5jdGlvbiBnZXRXb3JsZFBvc2l0aW9uTGl0ZShvYmplY3Q6IFRIUkVFLk9iamVjdDNELCBvdXQ6IFRIUkVFLlZlY3RvcjMpOiBUSFJFRS5WZWN0b3IzIHtcbiAgb2JqZWN0Lm1hdHJpeFdvcmxkLmRlY29tcG9zZShvdXQsIF9yb3RhdGlvbiwgX3NjYWxlKTtcbiAgcmV0dXJuIG91dDtcbn1cblxuLyoqXG4gKiBFeHRyYWN0IHdvcmxkIHNjYWxlIG9mIGFuIG9iamVjdCBmcm9tIGl0cyB3b3JsZCBzcGFjZSBtYXRyaXgsIGluIGNoZWFwZXIgd2F5LlxuICpcbiAqIEBwYXJhbSBvYmplY3QgVGhlIG9iamVjdFxuICogQHBhcmFtIG91dCBUYXJnZXQgdmVjdG9yXG4gKi9cbmV4cG9ydCBmdW5jdGlvbiBnZXRXb3JsZFNjYWxlTGl0ZShvYmplY3Q6IFRIUkVFLk9iamVjdDNELCBvdXQ6IFRIUkVFLlZlY3RvcjMpOiBUSFJFRS5WZWN0b3IzIHtcbiAgb2JqZWN0Lm1hdHJpeFdvcmxkLmRlY29tcG9zZShfcG9zaXRpb24sIF9yb3RhdGlvbiwgb3V0KTtcbiAgcmV0dXJuIG91dDtcbn1cblxuLyoqXG4gKiBFeHRyYWN0IHdvcmxkIHJvdGF0aW9uIG9mIGFuIG9iamVjdCBmcm9tIGl0cyB3b3JsZCBzcGFjZSBtYXRyaXgsIGluIGNoZWFwZXIgd2F5LlxuICpcbiAqIEBwYXJhbSBvYmplY3QgVGhlIG9iamVjdFxuICogQHBhcmFtIG91dCBUYXJnZXQgdmVjdG9yXG4gKi9cbmV4cG9ydCBmdW5jdGlvbiBnZXRXb3JsZFF1YXRlcm5pb25MaXRlKG9iamVjdDogVEhSRUUuT2JqZWN0M0QsIG91dDogVEhSRUUuUXVhdGVybmlvbik6IFRIUkVFLlF1YXRlcm5pb24ge1xuICBvYmplY3QubWF0cml4V29ybGQuZGVjb21wb3NlKF9wb3NpdGlvbiwgb3V0LCBfc2NhbGUpO1xuICByZXR1cm4gb3V0O1xufVxuIiwiaW1wb3J0IHsgVlJNU2NoZW1hIH0gZnJvbSAnLi4vdHlwZXMnO1xuaW1wb3J0IHsgc2F0dXJhdGUgfSBmcm9tICcuLi91dGlscy9tYXRoJztcbmltcG9ydCB7IFZSTUJsZW5kU2hhcGVHcm91cCB9IGZyb20gJy4vVlJNQmxlbmRTaGFwZUdyb3VwJztcblxuZXhwb3J0IGNsYXNzIFZSTUJsZW5kU2hhcGVQcm94eSB7XG4gIC8qKlxuICAgKiBMaXN0IG9mIHJlZ2lzdGVyZWQgYmxlbmQgc2hhcGUuXG4gICAqL1xuICBwcml2YXRlIHJlYWRvbmx5IF9ibGVuZFNoYXBlR3JvdXBzOiB7IFtuYW1lOiBzdHJpbmddOiBWUk1CbGVuZFNoYXBlR3JvdXAgfSA9IHt9O1xuXG4gIC8qKlxuICAgKiBBIG1hcCBmcm9tIFtbVlJNU2NoZW1hLkJsZW5kU2hhcGVQcmVzZXROYW1lXV0gdG8gaXRzIGFjdHVhbCBibGVuZCBzaGFwZSBuYW1lLlxuICAgKi9cbiAgcHJpdmF0ZSByZWFkb25seSBfYmxlbmRTaGFwZVByZXNldE1hcDogeyBbcHJlc2V0TmFtZSBpbiBWUk1TY2hlbWEuQmxlbmRTaGFwZVByZXNldE5hbWVdPzogc3RyaW5nIH0gPSB7fTtcblxuICAvKipcbiAgICogQSBsaXN0IG9mIG5hbWUgb2YgdW5rbm93biBibGVuZCBzaGFwZXMuXG4gICAqL1xuICBwcml2YXRlIHJlYWRvbmx5IF91bmtub3duR3JvdXBOYW1lczogc3RyaW5nW10gPSBbXTtcblxuICAvKipcbiAgICogQ3JlYXRlIGEgbmV3IFZSTUJsZW5kU2hhcGUuXG4gICAqL1xuICBwdWJsaWMgY29uc3RydWN0b3IoKSB7XG4gICAgLy8gZG8gbm90aGluZ1xuICB9XG5cbiAgLyoqXG4gICAqIExpc3Qgb2YgbmFtZSBvZiByZWdpc3RlcmVkIGJsZW5kIHNoYXBlIGdyb3VwLlxuICAgKi9cbiAgcHVibGljIGdldCBleHByZXNzaW9ucygpOiBzdHJpbmdbXSB7XG4gICAgcmV0dXJuIE9iamVjdC5rZXlzKHRoaXMuX2JsZW5kU2hhcGVHcm91cHMpO1xuICB9XG5cbiAgLyoqXG4gICAqIEEgbWFwIGZyb20gW1tWUk1TY2hlbWEuQmxlbmRTaGFwZVByZXNldE5hbWVdXSB0byBpdHMgYWN0dWFsIGJsZW5kIHNoYXBlIG5hbWUuXG4gICAqL1xuICBwdWJsaWMgZ2V0IGJsZW5kU2hhcGVQcmVzZXRNYXAoKTogeyBbcHJlc2V0TmFtZSBpbiBWUk1TY2hlbWEuQmxlbmRTaGFwZVByZXNldE5hbWVdPzogc3RyaW5nIH0ge1xuICAgIHJldHVybiB0aGlzLl9ibGVuZFNoYXBlUHJlc2V0TWFwO1xuICB9XG5cbiAgLyoqXG4gICAqIEEgbGlzdCBvZiBuYW1lIG9mIHVua25vd24gYmxlbmQgc2hhcGVzLlxuICAgKi9cbiAgcHVibGljIGdldCB1bmtub3duR3JvdXBOYW1lcygpOiBzdHJpbmdbXSB7XG4gICAgcmV0dXJuIHRoaXMuX3Vua25vd25Hcm91cE5hbWVzO1xuICB9XG5cbiAgLyoqXG4gICAqIFJldHVybiByZWdpc3RlcmVkIGJsZW5kIHNoYXBlIGdyb3VwLlxuICAgKlxuICAgKiBAcGFyYW0gbmFtZSBOYW1lIG9mIHRoZSBibGVuZCBzaGFwZSBncm91cFxuICAgKi9cbiAgcHVibGljIGdldEJsZW5kU2hhcGVHcm91cChuYW1lOiBzdHJpbmcgfCBWUk1TY2hlbWEuQmxlbmRTaGFwZVByZXNldE5hbWUpOiBWUk1CbGVuZFNoYXBlR3JvdXAgfCB1bmRlZmluZWQge1xuICAgIGNvbnN0IHByZXNldE5hbWUgPSB0aGlzLl9ibGVuZFNoYXBlUHJlc2V0TWFwW25hbWUgYXMgVlJNU2NoZW1hLkJsZW5kU2hhcGVQcmVzZXROYW1lXTtcbiAgICBjb25zdCBjb250cm9sbGVyID0gcHJlc2V0TmFtZSA/IHRoaXMuX2JsZW5kU2hhcGVHcm91cHNbcHJlc2V0TmFtZV0gOiB0aGlzLl9ibGVuZFNoYXBlR3JvdXBzW25hbWVdO1xuICAgIGlmICghY29udHJvbGxlcikge1xuICAgICAgY29uc29sZS53YXJuKGBubyBibGVuZCBzaGFwZSBmb3VuZCBieSAke25hbWV9YCk7XG4gICAgICByZXR1cm4gdW5kZWZpbmVkO1xuICAgIH1cbiAgICByZXR1cm4gY29udHJvbGxlcjtcbiAgfVxuXG4gIC8qKlxuICAgKiBSZWdpc3RlciBhIGJsZW5kIHNoYXBlIGdyb3VwLlxuICAgKlxuICAgKiBAcGFyYW0gbmFtZSBOYW1lIG9mIHRoZSBibGVuZCBzaGFwZSBnb3J1cFxuICAgKiBAcGFyYW0gY29udHJvbGxlciBWUk1CbGVuZFNoYXBlQ29udHJvbGxlciB0aGF0IGRlc2NyaWJlcyB0aGUgYmxlbmQgc2hhcGUgZ3JvdXBcbiAgICovXG4gIHB1YmxpYyByZWdpc3RlckJsZW5kU2hhcGVHcm91cChcbiAgICBuYW1lOiBzdHJpbmcsXG4gICAgcHJlc2V0TmFtZTogVlJNU2NoZW1hLkJsZW5kU2hhcGVQcmVzZXROYW1lIHwgdW5kZWZpbmVkLFxuICAgIGNvbnRyb2xsZXI6IFZSTUJsZW5kU2hhcGVHcm91cCxcbiAgKTogdm9pZCB7XG4gICAgdGhpcy5fYmxlbmRTaGFwZUdyb3Vwc1tuYW1lXSA9IGNvbnRyb2xsZXI7XG4gICAgaWYgKHByZXNldE5hbWUpIHtcbiAgICAgIHRoaXMuX2JsZW5kU2hhcGVQcmVzZXRNYXBbcHJlc2V0TmFtZV0gPSBuYW1lO1xuICAgIH0gZWxzZSB7XG4gICAgICB0aGlzLl91bmtub3duR3JvdXBOYW1lcy5wdXNoKG5hbWUpO1xuICAgIH1cbiAgfVxuXG4gIC8qKlxuICAgKiBHZXQgY3VycmVudCB3ZWlnaHQgb2Ygc3BlY2lmaWVkIGJsZW5kIHNoYXBlIGdyb3VwLlxuICAgKlxuICAgKiBAcGFyYW0gbmFtZSBOYW1lIG9mIHRoZSBibGVuZCBzaGFwZSBncm91cFxuICAgKi9cbiAgcHVibGljIGdldFZhbHVlKG5hbWU6IFZSTVNjaGVtYS5CbGVuZFNoYXBlUHJlc2V0TmFtZSB8IHN0cmluZyk6IG51bWJlciB8IG51bGwge1xuICAgIGNvbnN0IGNvbnRyb2xsZXIgPSB0aGlzLmdldEJsZW5kU2hhcGVHcm91cChuYW1lKTtcbiAgICByZXR1cm4gY29udHJvbGxlcj8ud2VpZ2h0ID8/IG51bGw7XG4gIH1cblxuICAvKipcbiAgICogU2V0IGEgd2VpZ2h0IHRvIHNwZWNpZmllZCBibGVuZCBzaGFwZSBncm91cC5cbiAgICpcbiAgICogQHBhcmFtIG5hbWUgTmFtZSBvZiB0aGUgYmxlbmQgc2hhcGUgZ3JvdXBcbiAgICogQHBhcmFtIHdlaWdodCBXZWlnaHRcbiAgICovXG4gIHB1YmxpYyBzZXRWYWx1ZShuYW1lOiBWUk1TY2hlbWEuQmxlbmRTaGFwZVByZXNldE5hbWUgfCBzdHJpbmcsIHdlaWdodDogbnVtYmVyKTogdm9pZCB7XG4gICAgY29uc3QgY29udHJvbGxlciA9IHRoaXMuZ2V0QmxlbmRTaGFwZUdyb3VwKG5hbWUpO1xuICAgIGlmIChjb250cm9sbGVyKSB7XG4gICAgICBjb250cm9sbGVyLndlaWdodCA9IHNhdHVyYXRlKHdlaWdodCk7XG4gICAgfVxuICB9XG5cbiAgLyoqXG4gICAqIEdldCBhIHRyYWNrIG5hbWUgb2Ygc3BlY2lmaWVkIGJsZW5kIHNoYXBlIGdyb3VwLlxuICAgKiBUaGlzIHRyYWNrIG5hbWUgaXMgbmVlZGVkIHRvIG1hbmlwdWxhdGUgaXRzIGJsZW5kIHNoYXBlIGdyb3VwIHZpYSBrZXlmcmFtZSBhbmltYXRpb25zLlxuICAgKlxuICAgKiBAZXhhbXBsZSBNYW5pcHVsYXRlIGEgYmxlbmQgc2hhcGUgZ3JvdXAgdXNpbmcga2V5ZnJhbWUgYW5pbWF0aW9uXG4gICAqIGBgYGpzXG4gICAqIGNvbnN0IHRyYWNrTmFtZSA9IHZybS5ibGVuZFNoYXBlUHJveHkuZ2V0QmxlbmRTaGFwZVRyYWNrTmFtZSggVEhSRUUuVlJNU2NoZW1hLkJsZW5kU2hhcGVQcmVzZXROYW1lLkJsaW5rICk7XG4gICAqIGNvbnN0IHRyYWNrID0gbmV3IFRIUkVFLk51bWJlcktleWZyYW1lVHJhY2soXG4gICAqICAgbmFtZSxcbiAgICogICBbIDAuMCwgMC41LCAxLjAgXSwgLy8gdGltZXNcbiAgICogICBbIDAuMCwgMS4wLCAwLjAgXSAvLyB2YWx1ZXNcbiAgICogKTtcbiAgICpcbiAgICogY29uc3QgY2xpcCA9IG5ldyBUSFJFRS5BbmltYXRpb25DbGlwKFxuICAgKiAgICdibGluaycsIC8vIG5hbWVcbiAgICogICAxLjAsIC8vIGR1cmF0aW9uXG4gICAqICAgWyB0cmFjayBdIC8vIHRyYWNrc1xuICAgKiApO1xuICAgKlxuICAgKiBjb25zdCBtaXhlciA9IG5ldyBUSFJFRS5BbmltYXRpb25NaXhlciggdnJtLnNjZW5lICk7XG4gICAqIGNvbnN0IGFjdGlvbiA9IG1peGVyLmNsaXBBY3Rpb24oIGNsaXAgKTtcbiAgICogYWN0aW9uLnBsYXkoKTtcbiAgICogYGBgXG4gICAqXG4gICAqIEBwYXJhbSBuYW1lIE5hbWUgb2YgdGhlIGJsZW5kIHNoYXBlIGdyb3VwXG4gICAqL1xuICBwdWJsaWMgZ2V0QmxlbmRTaGFwZVRyYWNrTmFtZShuYW1lOiBWUk1TY2hlbWEuQmxlbmRTaGFwZVByZXNldE5hbWUgfCBzdHJpbmcpOiBzdHJpbmcgfCBudWxsIHtcbiAgICBjb25zdCBjb250cm9sbGVyID0gdGhpcy5nZXRCbGVuZFNoYXBlR3JvdXAobmFtZSk7XG4gICAgcmV0dXJuIGNvbnRyb2xsZXIgPyBgJHtjb250cm9sbGVyLm5hbWV9LndlaWdodGAgOiBudWxsO1xuICB9XG5cbiAgLyoqXG4gICAqIFVwZGF0ZSBldmVyeSBibGVuZCBzaGFwZSBncm91cHMuXG4gICAqL1xuICBwdWJsaWMgdXBkYXRlKCk6IHZvaWQge1xuICAgIE9iamVjdC5rZXlzKHRoaXMuX2JsZW5kU2hhcGVHcm91cHMpLmZvckVhY2goKG5hbWUpID0+IHtcbiAgICAgIGNvbnN0IGNvbnRyb2xsZXIgPSB0aGlzLl9ibGVuZFNoYXBlR3JvdXBzW25hbWVdO1xuICAgICAgY29udHJvbGxlci5jbGVhckFwcGxpZWRXZWlnaHQoKTtcbiAgICB9KTtcblxuICAgIE9iamVjdC5rZXlzKHRoaXMuX2JsZW5kU2hhcGVHcm91cHMpLmZvckVhY2goKG5hbWUpID0+IHtcbiAgICAgIGNvbnN0IGNvbnRyb2xsZXIgPSB0aGlzLl9ibGVuZFNoYXBlR3JvdXBzW25hbWVdO1xuICAgICAgY29udHJvbGxlci5hcHBseVdlaWdodCgpO1xuICAgIH0pO1xuICB9XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5pbXBvcnQgeyBHTFRGIH0gZnJvbSAndGhyZWUvZXhhbXBsZXMvanNtL2xvYWRlcnMvR0xURkxvYWRlcic7XG5pbXBvcnQgeyBHTFRGU2NoZW1hLCBWUk1TY2hlbWEgfSBmcm9tICcuLi90eXBlcyc7XG5pbXBvcnQgeyBnbHRmRXh0cmFjdFByaW1pdGl2ZXNGcm9tTm9kZSB9IGZyb20gJy4uL3V0aWxzL2dsdGZFeHRyYWN0UHJpbWl0aXZlc0Zyb21Ob2RlJztcbmltcG9ydCB7IHJlbmFtZU1hdGVyaWFsUHJvcGVydHkgfSBmcm9tICcuLi91dGlscy9yZW5hbWVNYXRlcmlhbFByb3BlcnR5JztcbmltcG9ydCB7IFZSTUJsZW5kU2hhcGVHcm91cCB9IGZyb20gJy4vVlJNQmxlbmRTaGFwZUdyb3VwJztcbmltcG9ydCB7IFZSTUJsZW5kU2hhcGVQcm94eSB9IGZyb20gJy4vVlJNQmxlbmRTaGFwZVByb3h5JztcblxuLyoqXG4gKiBBbiBpbXBvcnRlciB0aGF0IGltcG9ydHMgYSBbW1ZSTUJsZW5kU2hhcGVdXSBmcm9tIGEgVlJNIGV4dGVuc2lvbiBvZiBhIEdMVEYuXG4gKi9cbmV4cG9ydCBjbGFzcyBWUk1CbGVuZFNoYXBlSW1wb3J0ZXIge1xuICAvKipcbiAgICogSW1wb3J0IGEgW1tWUk1CbGVuZFNoYXBlXV0gZnJvbSBhIFZSTS5cbiAgICpcbiAgICogQHBhcmFtIGdsdGYgQSBwYXJzZWQgcmVzdWx0IG9mIEdMVEYgdGFrZW4gZnJvbSBHTFRGTG9hZGVyXG4gICAqL1xuICBwdWJsaWMgYXN5bmMgaW1wb3J0KGdsdGY6IEdMVEYpOiBQcm9taXNlPFZSTUJsZW5kU2hhcGVQcm94eSB8IG51bGw+IHtcbiAgICBjb25zdCB2cm1FeHQ6IFZSTVNjaGVtYS5WUk0gfCB1bmRlZmluZWQgPSBnbHRmLnBhcnNlci5qc29uLmV4dGVuc2lvbnM/LlZSTTtcbiAgICBpZiAoIXZybUV4dCkge1xuICAgICAgcmV0dXJuIG51bGw7XG4gICAgfVxuXG4gICAgY29uc3Qgc2NoZW1hQmxlbmRTaGFwZTogVlJNU2NoZW1hLkJsZW5kU2hhcGUgfCB1bmRlZmluZWQgPSB2cm1FeHQuYmxlbmRTaGFwZU1hc3RlcjtcbiAgICBpZiAoIXNjaGVtYUJsZW5kU2hhcGUpIHtcbiAgICAgIHJldHVybiBudWxsO1xuICAgIH1cblxuICAgIGNvbnN0IGJsZW5kU2hhcGUgPSBuZXcgVlJNQmxlbmRTaGFwZVByb3h5KCk7XG5cbiAgICBjb25zdCBibGVuZFNoYXBlR3JvdXBzOiBWUk1TY2hlbWEuQmxlbmRTaGFwZUdyb3VwW10gfCB1bmRlZmluZWQgPSBzY2hlbWFCbGVuZFNoYXBlLmJsZW5kU2hhcGVHcm91cHM7XG4gICAgaWYgKCFibGVuZFNoYXBlR3JvdXBzKSB7XG4gICAgICByZXR1cm4gYmxlbmRTaGFwZTtcbiAgICB9XG5cbiAgICBjb25zdCBibGVuZFNoYXBlUHJlc2V0TWFwOiB7IFtwcmVzZXROYW1lIGluIFZSTVNjaGVtYS5CbGVuZFNoYXBlUHJlc2V0TmFtZV0/OiBzdHJpbmcgfSA9IHt9O1xuXG4gICAgYXdhaXQgUHJvbWlzZS5hbGwoXG4gICAgICBibGVuZFNoYXBlR3JvdXBzLm1hcChhc3luYyAoc2NoZW1hR3JvdXApID0+IHtcbiAgICAgICAgY29uc3QgbmFtZSA9IHNjaGVtYUdyb3VwLm5hbWU7XG4gICAgICAgIGlmIChuYW1lID09PSB1bmRlZmluZWQpIHtcbiAgICAgICAgICBjb25zb2xlLndhcm4oJ1ZSTUJsZW5kU2hhcGVJbXBvcnRlcjogT25lIG9mIGJsZW5kU2hhcGVHcm91cHMgaGFzIG5vIG5hbWUnKTtcbiAgICAgICAgICByZXR1cm47XG4gICAgICAgIH1cblxuICAgICAgICBsZXQgcHJlc2V0TmFtZTogVlJNU2NoZW1hLkJsZW5kU2hhcGVQcmVzZXROYW1lIHwgdW5kZWZpbmVkO1xuICAgICAgICBpZiAoXG4gICAgICAgICAgc2NoZW1hR3JvdXAucHJlc2V0TmFtZSAmJlxuICAgICAgICAgIHNjaGVtYUdyb3VwLnByZXNldE5hbWUgIT09IFZSTVNjaGVtYS5CbGVuZFNoYXBlUHJlc2V0TmFtZS5Vbmtub3duICYmXG4gICAgICAgICAgIWJsZW5kU2hhcGVQcmVzZXRNYXBbc2NoZW1hR3JvdXAucHJlc2V0TmFtZV1cbiAgICAgICAgKSB7XG4gICAgICAgICAgcHJlc2V0TmFtZSA9IHNjaGVtYUdyb3VwLnByZXNldE5hbWU7XG4gICAgICAgICAgYmxlbmRTaGFwZVByZXNldE1hcFtzY2hlbWFHcm91cC5wcmVzZXROYW1lXSA9IG5hbWU7XG4gICAgICAgIH1cblxuICAgICAgICBjb25zdCBncm91cCA9IG5ldyBWUk1CbGVuZFNoYXBlR3JvdXAobmFtZSk7XG4gICAgICAgIGdsdGYuc2NlbmUuYWRkKGdyb3VwKTtcblxuICAgICAgICBncm91cC5pc0JpbmFyeSA9IHNjaGVtYUdyb3VwLmlzQmluYXJ5IHx8IGZhbHNlO1xuXG4gICAgICAgIGlmIChzY2hlbWFHcm91cC5iaW5kcykge1xuICAgICAgICAgIHNjaGVtYUdyb3VwLmJpbmRzLmZvckVhY2goYXN5bmMgKGJpbmQpID0+IHtcbiAgICAgICAgICAgIGlmIChiaW5kLm1lc2ggPT09IHVuZGVmaW5lZCB8fCBiaW5kLmluZGV4ID09PSB1bmRlZmluZWQpIHtcbiAgICAgICAgICAgICAgcmV0dXJuO1xuICAgICAgICAgICAgfVxuXG4gICAgICAgICAgICBjb25zdCBub2Rlc1VzaW5nTWVzaDogbnVtYmVyW10gPSBbXTtcbiAgICAgICAgICAgIChnbHRmLnBhcnNlci5qc29uLm5vZGVzIGFzIEdMVEZTY2hlbWEuTm9kZVtdKS5mb3JFYWNoKChub2RlLCBpKSA9PiB7XG4gICAgICAgICAgICAgIGlmIChub2RlLm1lc2ggPT09IGJpbmQubWVzaCkge1xuICAgICAgICAgICAgICAgIG5vZGVzVXNpbmdNZXNoLnB1c2goaSk7XG4gICAgICAgICAgICAgIH1cbiAgICAgICAgICAgIH0pO1xuXG4gICAgICAgICAgICBjb25zdCBtb3JwaFRhcmdldEluZGV4ID0gYmluZC5pbmRleDtcblxuICAgICAgICAgICAgYXdhaXQgUHJvbWlzZS5hbGwoXG4gICAgICAgICAgICAgIG5vZGVzVXNpbmdNZXNoLm1hcChhc3luYyAobm9kZUluZGV4KSA9PiB7XG4gICAgICAgICAgICAgICAgY29uc3QgcHJpbWl0aXZlcyA9IChhd2FpdCBnbHRmRXh0cmFjdFByaW1pdGl2ZXNGcm9tTm9kZShnbHRmLCBub2RlSW5kZXgpKSE7XG5cbiAgICAgICAgICAgICAgICAvLyBjaGVjayBpZiB0aGUgbWVzaCBoYXMgdGhlIHRhcmdldCBtb3JwaCB0YXJnZXRcbiAgICAgICAgICAgICAgICBpZiAoXG4gICAgICAgICAgICAgICAgICAhcHJpbWl0aXZlcy5ldmVyeShcbiAgICAgICAgICAgICAgICAgICAgKHByaW1pdGl2ZSkgPT5cbiAgICAgICAgICAgICAgICAgICAgICBBcnJheS5pc0FycmF5KHByaW1pdGl2ZS5tb3JwaFRhcmdldEluZmx1ZW5jZXMpICYmXG4gICAgICAgICAgICAgICAgICAgICAgbW9ycGhUYXJnZXRJbmRleCA8IHByaW1pdGl2ZS5tb3JwaFRhcmdldEluZmx1ZW5jZXMubGVuZ3RoLFxuICAgICAgICAgICAgICAgICAgKVxuICAgICAgICAgICAgICAgICkge1xuICAgICAgICAgICAgICAgICAgY29uc29sZS53YXJuKFxuICAgICAgICAgICAgICAgICAgICBgVlJNQmxlbmRTaGFwZUltcG9ydGVyOiAke3NjaGVtYUdyb3VwLm5hbWV9IGF0dGVtcHRzIHRvIGluZGV4ICR7bW9ycGhUYXJnZXRJbmRleH10aCBtb3JwaCBidXQgbm90IGZvdW5kLmAsXG4gICAgICAgICAgICAgICAgICApO1xuICAgICAgICAgICAgICAgICAgcmV0dXJuO1xuICAgICAgICAgICAgICAgIH1cblxuICAgICAgICAgICAgICAgIGdyb3VwLmFkZEJpbmQoe1xuICAgICAgICAgICAgICAgICAgbWVzaGVzOiBwcmltaXRpdmVzLFxuICAgICAgICAgICAgICAgICAgbW9ycGhUYXJnZXRJbmRleCxcbiAgICAgICAgICAgICAgICAgIHdlaWdodDogYmluZC53ZWlnaHQgPz8gMTAwLFxuICAgICAgICAgICAgICAgIH0pO1xuICAgICAgICAgICAgICB9KSxcbiAgICAgICAgICAgICk7XG4gICAgICAgICAgfSk7XG4gICAgICAgIH1cblxuICAgICAgICBjb25zdCBtYXRlcmlhbFZhbHVlcyA9IHNjaGVtYUdyb3VwLm1hdGVyaWFsVmFsdWVzO1xuICAgICAgICBpZiAobWF0ZXJpYWxWYWx1ZXMpIHtcbiAgICAgICAgICBtYXRlcmlhbFZhbHVlcy5mb3JFYWNoKChtYXRlcmlhbFZhbHVlKSA9PiB7XG4gICAgICAgICAgICBpZiAoXG4gICAgICAgICAgICAgIG1hdGVyaWFsVmFsdWUubWF0ZXJpYWxOYW1lID09PSB1bmRlZmluZWQgfHxcbiAgICAgICAgICAgICAgbWF0ZXJpYWxWYWx1ZS5wcm9wZXJ0eU5hbWUgPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgICAgICBtYXRlcmlhbFZhbHVlLnRhcmdldFZhbHVlID09PSB1bmRlZmluZWRcbiAgICAgICAgICAgICkge1xuICAgICAgICAgICAgICByZXR1cm47XG4gICAgICAgICAgICB9XG5cbiAgICAgICAgICAgIGNvbnN0IG1hdGVyaWFsczogVEhSRUUuTWF0ZXJpYWxbXSA9IFtdO1xuICAgICAgICAgICAgZ2x0Zi5zY2VuZS50cmF2ZXJzZSgob2JqZWN0KSA9PiB7XG4gICAgICAgICAgICAgIGlmICgob2JqZWN0IGFzIGFueSkubWF0ZXJpYWwpIHtcbiAgICAgICAgICAgICAgICBjb25zdCBtYXRlcmlhbDogVEhSRUUuTWF0ZXJpYWxbXSB8IFRIUkVFLk1hdGVyaWFsID0gKG9iamVjdCBhcyBhbnkpLm1hdGVyaWFsO1xuICAgICAgICAgICAgICAgIGlmIChBcnJheS5pc0FycmF5KG1hdGVyaWFsKSkge1xuICAgICAgICAgICAgICAgICAgbWF0ZXJpYWxzLnB1c2goXG4gICAgICAgICAgICAgICAgICAgIC4uLm1hdGVyaWFsLmZpbHRlcihcbiAgICAgICAgICAgICAgICAgICAgICAobXRsKSA9PiBtdGwubmFtZSA9PT0gbWF0ZXJpYWxWYWx1ZS5tYXRlcmlhbE5hbWUhICYmIG1hdGVyaWFscy5pbmRleE9mKG10bCkgPT09IC0xLFxuICAgICAgICAgICAgICAgICAgICApLFxuICAgICAgICAgICAgICAgICAgKTtcbiAgICAgICAgICAgICAgICB9IGVsc2UgaWYgKG1hdGVyaWFsLm5hbWUgPT09IG1hdGVyaWFsVmFsdWUubWF0ZXJpYWxOYW1lICYmIG1hdGVyaWFscy5pbmRleE9mKG1hdGVyaWFsKSA9PT0gLTEpIHtcbiAgICAgICAgICAgICAgICAgIG1hdGVyaWFscy5wdXNoKG1hdGVyaWFsKTtcbiAgICAgICAgICAgICAgICB9XG4gICAgICAgICAgICAgIH1cbiAgICAgICAgICAgIH0pO1xuXG4gICAgICAgICAgICBtYXRlcmlhbHMuZm9yRWFjaCgobWF0ZXJpYWwpID0+IHtcbiAgICAgICAgICAgICAgZ3JvdXAuYWRkTWF0ZXJpYWxWYWx1ZSh7XG4gICAgICAgICAgICAgICAgbWF0ZXJpYWwsXG4gICAgICAgICAgICAgICAgcHJvcGVydHlOYW1lOiByZW5hbWVNYXRlcmlhbFByb3BlcnR5KG1hdGVyaWFsVmFsdWUucHJvcGVydHlOYW1lISksXG4gICAgICAgICAgICAgICAgdGFyZ2V0VmFsdWU6IG1hdGVyaWFsVmFsdWUudGFyZ2V0VmFsdWUhLFxuICAgICAgICAgICAgICB9KTtcbiAgICAgICAgICAgIH0pO1xuICAgICAgICAgIH0pO1xuICAgICAgICB9XG5cbiAgICAgICAgYmxlbmRTaGFwZS5yZWdpc3RlckJsZW5kU2hhcGVHcm91cChuYW1lLCBwcmVzZXROYW1lLCBncm91cCk7XG4gICAgICB9KSxcbiAgICApO1xuXG4gICAgcmV0dXJuIGJsZW5kU2hhcGU7XG4gIH1cbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcbmltcG9ydCB7IEdMVEZOb2RlLCBHTFRGUHJpbWl0aXZlIH0gZnJvbSAnLi4vdHlwZXMnO1xuaW1wb3J0IHsgZ2V0V29ybGRRdWF0ZXJuaW9uTGl0ZSB9IGZyb20gJy4uL3V0aWxzL21hdGgnO1xuXG5jb25zdCBWRUNUT1IzX0ZST05UID0gT2JqZWN0LmZyZWV6ZShuZXcgVEhSRUUuVmVjdG9yMygwLjAsIDAuMCwgLTEuMCkpO1xuXG5jb25zdCBfcXVhdCA9IG5ldyBUSFJFRS5RdWF0ZXJuaW9uKCk7XG5cbmVudW0gRmlyc3RQZXJzb25GbGFnIHtcbiAgQXV0byxcbiAgQm90aCxcbiAgVGhpcmRQZXJzb25Pbmx5LFxuICBGaXJzdFBlcnNvbk9ubHksXG59XG5cbi8qKlxuICogVGhpcyBjbGFzcyByZXByZXNlbnRzIGEgc2luZ2xlIFtgbWVzaEFubm90YXRpb25gXShodHRwczovL2dpdGh1Yi5jb20vdnJtLWMvVW5pVlJNL2Jsb2IvbWFzdGVyL3NwZWNpZmljYXRpb24vMC4wL3NjaGVtYS92cm0uZmlyc3RwZXJzb24ubWVzaGFubm90YXRpb24uc2NoZW1hLmpzb24pIGVudHJ5LlxuICogRWFjaCBtZXNoIHdpbGwgYmUgYXNzaWduZWQgdG8gc3BlY2lmaWVkIGxheWVyIHdoZW4geW91IGNhbGwgW1tWUk1GaXJzdFBlcnNvbi5zZXR1cF1dLlxuICovXG5leHBvcnQgY2xhc3MgVlJNUmVuZGVyZXJGaXJzdFBlcnNvbkZsYWdzIHtcbiAgcHJpdmF0ZSBzdGF0aWMgX3BhcnNlRmlyc3RQZXJzb25GbGFnKGZpcnN0UGVyc29uRmxhZzogc3RyaW5nIHwgdW5kZWZpbmVkKTogRmlyc3RQZXJzb25GbGFnIHtcbiAgICBzd2l0Y2ggKGZpcnN0UGVyc29uRmxhZykge1xuICAgICAgY2FzZSAnQm90aCc6XG4gICAgICAgIHJldHVybiBGaXJzdFBlcnNvbkZsYWcuQm90aDtcbiAgICAgIGNhc2UgJ1RoaXJkUGVyc29uT25seSc6XG4gICAgICAgIHJldHVybiBGaXJzdFBlcnNvbkZsYWcuVGhpcmRQZXJzb25Pbmx5O1xuICAgICAgY2FzZSAnRmlyc3RQZXJzb25Pbmx5JzpcbiAgICAgICAgcmV0dXJuIEZpcnN0UGVyc29uRmxhZy5GaXJzdFBlcnNvbk9ubHk7XG4gICAgICBkZWZhdWx0OlxuICAgICAgICByZXR1cm4gRmlyc3RQZXJzb25GbGFnLkF1dG87XG4gICAgfVxuICB9XG5cbiAgLyoqXG4gICAqIEEgW1tGaXJzdFBlcnNvbkZsYWddXSBvZiB0aGUgYW5ub3RhdGlvbiBlbnRyeS5cbiAgICovXG4gIHB1YmxpYyBmaXJzdFBlcnNvbkZsYWc6IEZpcnN0UGVyc29uRmxhZztcblxuICAvKipcbiAgICogQSBtZXNoIHByaW1pdGl2ZXMgb2YgdGhlIGFubm90YXRpb24gZW50cnkuXG4gICAqL1xuICBwdWJsaWMgcHJpbWl0aXZlczogR0xURlByaW1pdGl2ZVtdO1xuXG4gIC8qKlxuICAgKiBDcmVhdGUgYSBuZXcgbWVzaCBhbm5vdGF0aW9uLlxuICAgKlxuICAgKiBAcGFyYW0gZmlyc3RQZXJzb25GbGFnIEEgW1tGaXJzdFBlcnNvbkZsYWddXSBvZiB0aGUgYW5ub3RhdGlvbiBlbnRyeVxuICAgKiBAcGFyYW0gbm9kZSBBIG5vZGUgb2YgdGhlIGFubm90YXRpb24gZW50cnkuXG4gICAqL1xuICBjb25zdHJ1Y3RvcihmaXJzdFBlcnNvbkZsYWc6IHN0cmluZyB8IHVuZGVmaW5lZCwgcHJpbWl0aXZlczogR0xURlByaW1pdGl2ZVtdKSB7XG4gICAgdGhpcy5maXJzdFBlcnNvbkZsYWcgPSBWUk1SZW5kZXJlckZpcnN0UGVyc29uRmxhZ3MuX3BhcnNlRmlyc3RQZXJzb25GbGFnKGZpcnN0UGVyc29uRmxhZyk7XG4gICAgdGhpcy5wcmltaXRpdmVzID0gcHJpbWl0aXZlcztcbiAgfVxufVxuXG5leHBvcnQgY2xhc3MgVlJNRmlyc3RQZXJzb24ge1xuICAvKipcbiAgICogQSBkZWZhdWx0IGNhbWVyYSBsYXllciBmb3IgYEZpcnN0UGVyc29uT25seWAgbGF5ZXIuXG4gICAqXG4gICAqIEBzZWUgW1tnZXRGaXJzdFBlcnNvbk9ubHlMYXllcl1dXG4gICAqL1xuICBwcml2YXRlIHN0YXRpYyByZWFkb25seSBfREVGQVVMVF9GSVJTVFBFUlNPTl9PTkxZX0xBWUVSID0gOTtcblxuICAvKipcbiAgICogQSBkZWZhdWx0IGNhbWVyYSBsYXllciBmb3IgYFRoaXJkUGVyc29uT25seWAgbGF5ZXIuXG4gICAqXG4gICAqIEBzZWUgW1tnZXRUaGlyZFBlcnNvbk9ubHlMYXllcl1dXG4gICAqL1xuICBwcml2YXRlIHN0YXRpYyByZWFkb25seSBfREVGQVVMVF9USElSRFBFUlNPTl9PTkxZX0xBWUVSID0gMTA7XG5cbiAgcHJpdmF0ZSByZWFkb25seSBfZmlyc3RQZXJzb25Cb25lOiBHTFRGTm9kZTtcbiAgcHJpdmF0ZSByZWFkb25seSBfbWVzaEFubm90YXRpb25zOiBWUk1SZW5kZXJlckZpcnN0UGVyc29uRmxhZ3NbXSA9IFtdO1xuICBwcml2YXRlIHJlYWRvbmx5IF9maXJzdFBlcnNvbkJvbmVPZmZzZXQ6IFRIUkVFLlZlY3RvcjM7XG5cbiAgcHJpdmF0ZSBfZmlyc3RQZXJzb25Pbmx5TGF5ZXIgPSBWUk1GaXJzdFBlcnNvbi5fREVGQVVMVF9GSVJTVFBFUlNPTl9PTkxZX0xBWUVSO1xuICBwcml2YXRlIF90aGlyZFBlcnNvbk9ubHlMYXllciA9IFZSTUZpcnN0UGVyc29uLl9ERUZBVUxUX1RISVJEUEVSU09OX09OTFlfTEFZRVI7XG5cbiAgcHJpdmF0ZSBfaW5pdGlhbGl6ZWQgPSBmYWxzZTtcblxuICAvKipcbiAgICogQ3JlYXRlIGEgbmV3IFZSTUZpcnN0UGVyc29uIG9iamVjdC5cbiAgICpcbiAgICogQHBhcmFtIGZpcnN0UGVyc29uQm9uZSBBIGZpcnN0IHBlcnNvbiBib25lXG4gICAqIEBwYXJhbSBmaXJzdFBlcnNvbkJvbmVPZmZzZXQgQW4gb2Zmc2V0IGZyb20gdGhlIHNwZWNpZmllZCBmaXJzdCBwZXJzb24gYm9uZVxuICAgKiBAcGFyYW0gbWVzaEFubm90YXRpb25zIEEgcmVuZGVyZXIgc2V0dGluZ3MuIFNlZSB0aGUgZGVzY3JpcHRpb24gb2YgW1tSZW5kZXJlckZpcnN0UGVyc29uRmxhZ3NdXSBmb3IgbW9yZSBpbmZvXG4gICAqL1xuICBjb25zdHJ1Y3RvcihcbiAgICBmaXJzdFBlcnNvbkJvbmU6IEdMVEZOb2RlLFxuICAgIGZpcnN0UGVyc29uQm9uZU9mZnNldDogVEhSRUUuVmVjdG9yMyxcbiAgICBtZXNoQW5ub3RhdGlvbnM6IFZSTVJlbmRlcmVyRmlyc3RQZXJzb25GbGFnc1tdLFxuICApIHtcbiAgICB0aGlzLl9maXJzdFBlcnNvbkJvbmUgPSBmaXJzdFBlcnNvbkJvbmU7XG4gICAgdGhpcy5fZmlyc3RQZXJzb25Cb25lT2Zmc2V0ID0gZmlyc3RQZXJzb25Cb25lT2Zmc2V0O1xuICAgIHRoaXMuX21lc2hBbm5vdGF0aW9ucyA9IG1lc2hBbm5vdGF0aW9ucztcbiAgfVxuXG4gIHB1YmxpYyBnZXQgZmlyc3RQZXJzb25Cb25lKCk6IEdMVEZOb2RlIHtcbiAgICByZXR1cm4gdGhpcy5fZmlyc3RQZXJzb25Cb25lO1xuICB9XG5cbiAgcHVibGljIGdldCBtZXNoQW5ub3RhdGlvbnMoKTogVlJNUmVuZGVyZXJGaXJzdFBlcnNvbkZsYWdzW10ge1xuICAgIHJldHVybiB0aGlzLl9tZXNoQW5ub3RhdGlvbnM7XG4gIH1cblxuICBwdWJsaWMgZ2V0Rmlyc3RQZXJzb25Xb3JsZERpcmVjdGlvbih0YXJnZXQ6IFRIUkVFLlZlY3RvcjMpOiBUSFJFRS5WZWN0b3IzIHtcbiAgICByZXR1cm4gdGFyZ2V0LmNvcHkoVkVDVE9SM19GUk9OVCkuYXBwbHlRdWF0ZXJuaW9uKGdldFdvcmxkUXVhdGVybmlvbkxpdGUodGhpcy5fZmlyc3RQZXJzb25Cb25lLCBfcXVhdCkpO1xuICB9XG5cbiAgLyoqXG4gICAqIEEgY2FtZXJhIGxheWVyIHJlcHJlc2VudHMgYEZpcnN0UGVyc29uT25seWAgbGF5ZXIuXG4gICAqIE5vdGUgdGhhdCAqKnlvdSBtdXN0IGNhbGwgW1tzZXR1cF1dIGZpcnN0IGJlZm9yZSB5b3UgdXNlIHRoZSBsYXllciBmZWF0dXJlKiogb3IgaXQgZG9lcyBub3Qgd29yayBwcm9wZXJseS5cbiAgICpcbiAgICogVGhlIHZhbHVlIGlzIFtbREVGQVVMVF9GSVJTVFBFUlNPTl9PTkxZX0xBWUVSXV0gYnkgZGVmYXVsdCBidXQgeW91IGNhbiBjaGFuZ2UgdGhlIGxheWVyIGJ5IHNwZWNpZnlpbmcgdmlhIFtbc2V0dXBdXSBpZiB5b3UgcHJlZmVyLlxuICAgKlxuICAgKiBAc2VlIGh0dHBzOi8vdnJtLmRldi9lbi91bml2cm0vYXBpL3VuaXZybV91c2VfZmlyc3RwZXJzb24vXG4gICAqIEBzZWUgaHR0cHM6Ly90aHJlZWpzLm9yZy9kb2NzLyNhcGkvZW4vY29yZS9MYXllcnNcbiAgICovXG4gIHB1YmxpYyBnZXQgZmlyc3RQZXJzb25Pbmx5TGF5ZXIoKTogbnVtYmVyIHtcbiAgICByZXR1cm4gdGhpcy5fZmlyc3RQZXJzb25Pbmx5TGF5ZXI7XG4gIH1cblxuICAvKipcbiAgICogQSBjYW1lcmEgbGF5ZXIgcmVwcmVzZW50cyBgVGhpcmRQZXJzb25Pbmx5YCBsYXllci5cbiAgICogTm90ZSB0aGF0ICoqeW91IG11c3QgY2FsbCBbW3NldHVwXV0gZmlyc3QgYmVmb3JlIHlvdSB1c2UgdGhlIGxheWVyIGZlYXR1cmUqKiBvciBpdCBkb2VzIG5vdCB3b3JrIHByb3Blcmx5LlxuICAgKlxuICAgKiBUaGUgdmFsdWUgaXMgW1tERUZBVUxUX1RISVJEUEVSU09OX09OTFlfTEFZRVJdXSBieSBkZWZhdWx0IGJ1dCB5b3UgY2FuIGNoYW5nZSB0aGUgbGF5ZXIgYnkgc3BlY2lmeWluZyB2aWEgW1tzZXR1cF1dIGlmIHlvdSBwcmVmZXIuXG4gICAqXG4gICAqIEBzZWUgaHR0cHM6Ly92cm0uZGV2L2VuL3VuaXZybS9hcGkvdW5pdnJtX3VzZV9maXJzdHBlcnNvbi9cbiAgICogQHNlZSBodHRwczovL3RocmVlanMub3JnL2RvY3MvI2FwaS9lbi9jb3JlL0xheWVyc1xuICAgKi9cbiAgcHVibGljIGdldCB0aGlyZFBlcnNvbk9ubHlMYXllcigpOiBudW1iZXIge1xuICAgIHJldHVybiB0aGlzLl90aGlyZFBlcnNvbk9ubHlMYXllcjtcbiAgfVxuXG4gIHB1YmxpYyBnZXRGaXJzdFBlcnNvbkJvbmVPZmZzZXQodGFyZ2V0OiBUSFJFRS5WZWN0b3IzKTogVEhSRUUuVmVjdG9yMyB7XG4gICAgcmV0dXJuIHRhcmdldC5jb3B5KHRoaXMuX2ZpcnN0UGVyc29uQm9uZU9mZnNldCk7XG4gIH1cblxuICAvKipcbiAgICogR2V0IGN1cnJlbnQgd29ybGQgcG9zaXRpb24gb2YgdGhlIGZpcnN0IHBlcnNvbi5cbiAgICogVGhlIHBvc2l0aW9uIHRha2VzIFtbRmlyc3RQZXJzb25Cb25lXV0gYW5kIFtbRmlyc3RQZXJzb25PZmZzZXRdXSBpbnRvIGFjY291bnQuXG4gICAqXG4gICAqIEBwYXJhbSB2MyB0YXJnZXRcbiAgICogQHJldHVybnMgQ3VycmVudCB3b3JsZCBwb3NpdGlvbiBvZiB0aGUgZmlyc3QgcGVyc29uXG4gICAqL1xuICBwdWJsaWMgZ2V0Rmlyc3RQZXJzb25Xb3JsZFBvc2l0aW9uKHYzOiBUSFJFRS5WZWN0b3IzKTogVEhSRUUuVmVjdG9yMyB7XG4gICAgLy8gVW5pVlJNI1ZSTUZpcnN0UGVyc29uRWRpdG9yXG4gICAgLy8gdmFyIHdvcmxkT2Zmc2V0ID0gaGVhZC5sb2NhbFRvV29ybGRNYXRyaXguTXVsdGlwbHlQb2ludChjb21wb25lbnQuRmlyc3RQZXJzb25PZmZzZXQpO1xuICAgIGNvbnN0IG9mZnNldCA9IHRoaXMuX2ZpcnN0UGVyc29uQm9uZU9mZnNldDtcbiAgICBjb25zdCB2NCA9IG5ldyBUSFJFRS5WZWN0b3I0KG9mZnNldC54LCBvZmZzZXQueSwgb2Zmc2V0LnosIDEuMCk7XG4gICAgdjQuYXBwbHlNYXRyaXg0KHRoaXMuX2ZpcnN0UGVyc29uQm9uZS5tYXRyaXhXb3JsZCk7XG4gICAgcmV0dXJuIHYzLnNldCh2NC54LCB2NC55LCB2NC56KTtcbiAgfVxuXG4gIC8qKlxuICAgKiBJbiB0aGlzIG1ldGhvZCwgaXQgYXNzaWducyBsYXllcnMgZm9yIGV2ZXJ5IG1lc2hlcyBiYXNlZCBvbiBtZXNoIGFubm90YXRpb25zLlxuICAgKiBZb3UgbXVzdCBjYWxsIHRoaXMgbWV0aG9kIGZpcnN0IGJlZm9yZSB5b3UgdXNlIHRoZSBsYXllciBmZWF0dXJlLlxuICAgKlxuICAgKiBUaGlzIGlzIGFuIGVxdWl2YWxlbnQgb2YgW1ZSTUZpcnN0UGVyc29uLlNldHVwXShodHRwczovL2dpdGh1Yi5jb20vdnJtLWMvVW5pVlJNL2Jsb2IvbWFzdGVyL0Fzc2V0cy9WUk0vVW5pVlJNL1NjcmlwdHMvRmlyc3RQZXJzb24vVlJNRmlyc3RQZXJzb24uY3MpIG9mIHRoZSBVbmlWUk0uXG4gICAqXG4gICAqIFRoZSBgY2FtZXJhTGF5ZXJgIHBhcmFtZXRlciBzcGVjaWZpZXMgd2hpY2ggbGF5ZXIgd2lsbCBiZSBhc3NpZ25lZCBmb3IgYEZpcnN0UGVyc29uT25seWAgLyBgVGhpcmRQZXJzb25Pbmx5YC5cbiAgICogSW4gVW5pVlJNLCB3ZSBzcGVjaWZpZWQgdGhvc2UgYnkgbmFtaW5nIGVhY2ggZGVzaXJlZCBsYXllciBhcyBgRklSU1RQRVJTT05fT05MWV9MQVlFUmAgLyBgVEhJUkRQRVJTT05fT05MWV9MQVlFUmBcbiAgICogYnV0IHdlIGFyZSBnb2luZyB0byBzcGVjaWZ5IHRoZXNlIGxheWVycyBhdCBoZXJlIHNpbmNlIHdlIGFyZSB1bmFibGUgdG8gbmFtZSBsYXllcnMgaW4gVGhyZWUuanMuXG4gICAqXG4gICAqIEBwYXJhbSBjYW1lcmFMYXllciBTcGVjaWZ5IHdoaWNoIGxheWVyIHdpbGwgYmUgZm9yIGBGaXJzdFBlcnNvbk9ubHlgIC8gYFRoaXJkUGVyc29uT25seWAuXG4gICAqL1xuICBwdWJsaWMgc2V0dXAoe1xuICAgIGZpcnN0UGVyc29uT25seUxheWVyID0gVlJNRmlyc3RQZXJzb24uX0RFRkFVTFRfRklSU1RQRVJTT05fT05MWV9MQVlFUixcbiAgICB0aGlyZFBlcnNvbk9ubHlMYXllciA9IFZSTUZpcnN0UGVyc29uLl9ERUZBVUxUX1RISVJEUEVSU09OX09OTFlfTEFZRVIsXG4gIH0gPSB7fSk6IHZvaWQge1xuICAgIGlmICh0aGlzLl9pbml0aWFsaXplZCkge1xuICAgICAgcmV0dXJuO1xuICAgIH1cbiAgICB0aGlzLl9pbml0aWFsaXplZCA9IHRydWU7XG4gICAgdGhpcy5fZmlyc3RQZXJzb25Pbmx5TGF5ZXIgPSBmaXJzdFBlcnNvbk9ubHlMYXllcjtcbiAgICB0aGlzLl90aGlyZFBlcnNvbk9ubHlMYXllciA9IHRoaXJkUGVyc29uT25seUxheWVyO1xuXG4gICAgdGhpcy5fbWVzaEFubm90YXRpb25zLmZvckVhY2goKGl0ZW0pID0+IHtcbiAgICAgIGlmIChpdGVtLmZpcnN0UGVyc29uRmxhZyA9PT0gRmlyc3RQZXJzb25GbGFnLkZpcnN0UGVyc29uT25seSkge1xuICAgICAgICBpdGVtLnByaW1pdGl2ZXMuZm9yRWFjaCgocHJpbWl0aXZlKSA9PiB7XG4gICAgICAgICAgcHJpbWl0aXZlLmxheWVycy5zZXQodGhpcy5fZmlyc3RQZXJzb25Pbmx5TGF5ZXIpO1xuICAgICAgICB9KTtcbiAgICAgIH0gZWxzZSBpZiAoaXRlbS5maXJzdFBlcnNvbkZsYWcgPT09IEZpcnN0UGVyc29uRmxhZy5UaGlyZFBlcnNvbk9ubHkpIHtcbiAgICAgICAgaXRlbS5wcmltaXRpdmVzLmZvckVhY2goKHByaW1pdGl2ZSkgPT4ge1xuICAgICAgICAgIHByaW1pdGl2ZS5sYXllcnMuc2V0KHRoaXMuX3RoaXJkUGVyc29uT25seUxheWVyKTtcbiAgICAgICAgfSk7XG4gICAgICB9IGVsc2UgaWYgKGl0ZW0uZmlyc3RQZXJzb25GbGFnID09PSBGaXJzdFBlcnNvbkZsYWcuQXV0bykge1xuICAgICAgICB0aGlzLl9jcmVhdGVIZWFkbGVzc01vZGVsKGl0ZW0ucHJpbWl0aXZlcyk7XG4gICAgICB9XG4gICAgfSk7XG4gIH1cblxuICBwcml2YXRlIF9leGNsdWRlVHJpYW5nbGVzKHRyaWFuZ2xlczogbnVtYmVyW10sIGJ3czogbnVtYmVyW11bXSwgc2tpbkluZGV4OiBudW1iZXJbXVtdLCBleGNsdWRlOiBudW1iZXJbXSk6IG51bWJlciB7XG4gICAgbGV0IGNvdW50ID0gMDtcbiAgICBpZiAoYndzICE9IG51bGwgJiYgYndzLmxlbmd0aCA+IDApIHtcbiAgICAgIGZvciAobGV0IGkgPSAwOyBpIDwgdHJpYW5nbGVzLmxlbmd0aDsgaSArPSAzKSB7XG4gICAgICAgIGNvbnN0IGEgPSB0cmlhbmdsZXNbaV07XG4gICAgICAgIGNvbnN0IGIgPSB0cmlhbmdsZXNbaSArIDFdO1xuICAgICAgICBjb25zdCBjID0gdHJpYW5nbGVzW2kgKyAyXTtcbiAgICAgICAgY29uc3QgYncwID0gYndzW2FdO1xuICAgICAgICBjb25zdCBza2luMCA9IHNraW5JbmRleFthXTtcblxuICAgICAgICBpZiAoYncwWzBdID4gMCAmJiBleGNsdWRlLmluY2x1ZGVzKHNraW4wWzBdKSkgY29udGludWU7XG4gICAgICAgIGlmIChidzBbMV0gPiAwICYmIGV4Y2x1ZGUuaW5jbHVkZXMoc2tpbjBbMV0pKSBjb250aW51ZTtcbiAgICAgICAgaWYgKGJ3MFsyXSA+IDAgJiYgZXhjbHVkZS5pbmNsdWRlcyhza2luMFsyXSkpIGNvbnRpbnVlO1xuICAgICAgICBpZiAoYncwWzNdID4gMCAmJiBleGNsdWRlLmluY2x1ZGVzKHNraW4wWzNdKSkgY29udGludWU7XG5cbiAgICAgICAgY29uc3QgYncxID0gYndzW2JdO1xuICAgICAgICBjb25zdCBza2luMSA9IHNraW5JbmRleFtiXTtcbiAgICAgICAgaWYgKGJ3MVswXSA+IDAgJiYgZXhjbHVkZS5pbmNsdWRlcyhza2luMVswXSkpIGNvbnRpbnVlO1xuICAgICAgICBpZiAoYncxWzFdID4gMCAmJiBleGNsdWRlLmluY2x1ZGVzKHNraW4xWzFdKSkgY29udGludWU7XG4gICAgICAgIGlmIChidzFbMl0gPiAwICYmIGV4Y2x1ZGUuaW5jbHVkZXMoc2tpbjFbMl0pKSBjb250aW51ZTtcbiAgICAgICAgaWYgKGJ3MVszXSA+IDAgJiYgZXhjbHVkZS5pbmNsdWRlcyhza2luMVszXSkpIGNvbnRpbnVlO1xuXG4gICAgICAgIGNvbnN0IGJ3MiA9IGJ3c1tjXTtcbiAgICAgICAgY29uc3Qgc2tpbjIgPSBza2luSW5kZXhbY107XG4gICAgICAgIGlmIChidzJbMF0gPiAwICYmIGV4Y2x1ZGUuaW5jbHVkZXMoc2tpbjJbMF0pKSBjb250aW51ZTtcbiAgICAgICAgaWYgKGJ3MlsxXSA+IDAgJiYgZXhjbHVkZS5pbmNsdWRlcyhza2luMlsxXSkpIGNvbnRpbnVlO1xuICAgICAgICBpZiAoYncyWzJdID4gMCAmJiBleGNsdWRlLmluY2x1ZGVzKHNraW4yWzJdKSkgY29udGludWU7XG4gICAgICAgIGlmIChidzJbM10gPiAwICYmIGV4Y2x1ZGUuaW5jbHVkZXMoc2tpbjJbM10pKSBjb250aW51ZTtcblxuICAgICAgICB0cmlhbmdsZXNbY291bnQrK10gPSBhO1xuICAgICAgICB0cmlhbmdsZXNbY291bnQrK10gPSBiO1xuICAgICAgICB0cmlhbmdsZXNbY291bnQrK10gPSBjO1xuICAgICAgfVxuICAgIH1cbiAgICByZXR1cm4gY291bnQ7XG4gIH1cblxuICBwcml2YXRlIF9jcmVhdGVFcmFzZWRNZXNoKHNyYzogVEhSRUUuU2tpbm5lZE1lc2gsIGVyYXNpbmdCb25lc0luZGV4OiBudW1iZXJbXSk6IFRIUkVFLlNraW5uZWRNZXNoIHtcbiAgICBjb25zdCBkc3QgPSBuZXcgVEhSRUUuU2tpbm5lZE1lc2goc3JjLmdlb21ldHJ5LmNsb25lKCksIHNyYy5tYXRlcmlhbCk7XG4gICAgZHN0Lm5hbWUgPSBgJHtzcmMubmFtZX0oZXJhc2UpYDtcbiAgICBkc3QuZnJ1c3R1bUN1bGxlZCA9IHNyYy5mcnVzdHVtQ3VsbGVkO1xuICAgIGRzdC5sYXllcnMuc2V0KHRoaXMuX2ZpcnN0UGVyc29uT25seUxheWVyKTtcblxuICAgIGNvbnN0IGdlb21ldHJ5ID0gZHN0Lmdlb21ldHJ5O1xuXG4gICAgY29uc3Qgc2tpbkluZGV4QXR0ciA9IGdlb21ldHJ5LmdldEF0dHJpYnV0ZSgnc2tpbkluZGV4JykuYXJyYXk7XG4gICAgY29uc3Qgc2tpbkluZGV4ID0gW107XG4gICAgZm9yIChsZXQgaSA9IDA7IGkgPCBza2luSW5kZXhBdHRyLmxlbmd0aDsgaSArPSA0KSB7XG4gICAgICBza2luSW5kZXgucHVzaChbc2tpbkluZGV4QXR0cltpXSwgc2tpbkluZGV4QXR0cltpICsgMV0sIHNraW5JbmRleEF0dHJbaSArIDJdLCBza2luSW5kZXhBdHRyW2kgKyAzXV0pO1xuICAgIH1cblxuICAgIGNvbnN0IHNraW5XZWlnaHRBdHRyID0gZ2VvbWV0cnkuZ2V0QXR0cmlidXRlKCdza2luV2VpZ2h0JykuYXJyYXk7XG4gICAgY29uc3Qgc2tpbldlaWdodCA9IFtdO1xuICAgIGZvciAobGV0IGkgPSAwOyBpIDwgc2tpbldlaWdodEF0dHIubGVuZ3RoOyBpICs9IDQpIHtcbiAgICAgIHNraW5XZWlnaHQucHVzaChbc2tpbldlaWdodEF0dHJbaV0sIHNraW5XZWlnaHRBdHRyW2kgKyAxXSwgc2tpbldlaWdodEF0dHJbaSArIDJdLCBza2luV2VpZ2h0QXR0cltpICsgM11dKTtcbiAgICB9XG5cbiAgICBjb25zdCBpbmRleCA9IGdlb21ldHJ5LmdldEluZGV4KCk7XG4gICAgaWYgKCFpbmRleCkge1xuICAgICAgdGhyb3cgbmV3IEVycm9yKFwiVGhlIGdlb21ldHJ5IGRvZXNuJ3QgaGF2ZSBhbiBpbmRleCBidWZmZXJcIik7XG4gICAgfVxuICAgIGNvbnN0IG9sZFRyaWFuZ2xlcyA9IEFycmF5LmZyb20oaW5kZXguYXJyYXkpO1xuXG4gICAgY29uc3QgY291bnQgPSB0aGlzLl9leGNsdWRlVHJpYW5nbGVzKG9sZFRyaWFuZ2xlcywgc2tpbldlaWdodCwgc2tpbkluZGV4LCBlcmFzaW5nQm9uZXNJbmRleCk7XG4gICAgY29uc3QgbmV3VHJpYW5nbGU6IG51bWJlcltdID0gW107XG4gICAgZm9yIChsZXQgaSA9IDA7IGkgPCBjb3VudDsgaSsrKSB7XG4gICAgICBuZXdUcmlhbmdsZVtpXSA9IG9sZFRyaWFuZ2xlc1tpXTtcbiAgICB9XG4gICAgZ2VvbWV0cnkuc2V0SW5kZXgobmV3VHJpYW5nbGUpO1xuXG4gICAgLy8gbXRvb24gbWF0ZXJpYWwgaW5jbHVkZXMgb25CZWZvcmVSZW5kZXIuIHRoaXMgaXMgdW5zdXBwb3J0ZWQgYXQgU2tpbm5lZE1lc2gjY2xvbmVcbiAgICBpZiAoc3JjLm9uQmVmb3JlUmVuZGVyKSB7XG4gICAgICBkc3Qub25CZWZvcmVSZW5kZXIgPSBzcmMub25CZWZvcmVSZW5kZXI7XG4gICAgfVxuICAgIGRzdC5iaW5kKG5ldyBUSFJFRS5Ta2VsZXRvbihzcmMuc2tlbGV0b24uYm9uZXMsIHNyYy5za2VsZXRvbi5ib25lSW52ZXJzZXMpLCBuZXcgVEhSRUUuTWF0cml4NCgpKTtcbiAgICByZXR1cm4gZHN0O1xuICB9XG5cbiAgcHJpdmF0ZSBfY3JlYXRlSGVhZGxlc3NNb2RlbEZvclNraW5uZWRNZXNoKHBhcmVudDogVEhSRUUuT2JqZWN0M0QsIG1lc2g6IFRIUkVFLlNraW5uZWRNZXNoKTogdm9pZCB7XG4gICAgY29uc3QgZXJhc2VCb25lSW5kZXhlczogbnVtYmVyW10gPSBbXTtcbiAgICBtZXNoLnNrZWxldG9uLmJvbmVzLmZvckVhY2goKGJvbmUsIGluZGV4KSA9PiB7XG4gICAgICBpZiAodGhpcy5faXNFcmFzZVRhcmdldChib25lKSkgZXJhc2VCb25lSW5kZXhlcy5wdXNoKGluZGV4KTtcbiAgICB9KTtcblxuICAgIC8vIFVubGlrZSBVbmlWUk0gd2UgZG9uJ3QgY29weSBtZXNoIGlmIG5vIGludmlzaWJsZSBib25lIHdhcyBmb3VuZFxuICAgIGlmICghZXJhc2VCb25lSW5kZXhlcy5sZW5ndGgpIHtcbiAgICAgIG1lc2gubGF5ZXJzLmVuYWJsZSh0aGlzLl90aGlyZFBlcnNvbk9ubHlMYXllcik7XG4gICAgICBtZXNoLmxheWVycy5lbmFibGUodGhpcy5fZmlyc3RQZXJzb25Pbmx5TGF5ZXIpO1xuICAgICAgcmV0dXJuO1xuICAgIH1cbiAgICBtZXNoLmxheWVycy5zZXQodGhpcy5fdGhpcmRQZXJzb25Pbmx5TGF5ZXIpO1xuICAgIGNvbnN0IG5ld01lc2ggPSB0aGlzLl9jcmVhdGVFcmFzZWRNZXNoKG1lc2gsIGVyYXNlQm9uZUluZGV4ZXMpO1xuICAgIHBhcmVudC5hZGQobmV3TWVzaCk7XG4gIH1cblxuICBwcml2YXRlIF9jcmVhdGVIZWFkbGVzc01vZGVsKHByaW1pdGl2ZXM6IEdMVEZQcmltaXRpdmVbXSk6IHZvaWQge1xuICAgIHByaW1pdGl2ZXMuZm9yRWFjaCgocHJpbWl0aXZlKSA9PiB7XG4gICAgICBpZiAocHJpbWl0aXZlLnR5cGUgPT09ICdTa2lubmVkTWVzaCcpIHtcbiAgICAgICAgY29uc3Qgc2tpbm5lZE1lc2ggPSBwcmltaXRpdmUgYXMgVEhSRUUuU2tpbm5lZE1lc2g7XG4gICAgICAgIHRoaXMuX2NyZWF0ZUhlYWRsZXNzTW9kZWxGb3JTa2lubmVkTWVzaChza2lubmVkTWVzaC5wYXJlbnQhLCBza2lubmVkTWVzaCk7XG4gICAgICB9IGVsc2Uge1xuICAgICAgICBpZiAodGhpcy5faXNFcmFzZVRhcmdldChwcmltaXRpdmUpKSB7XG4gICAgICAgICAgcHJpbWl0aXZlLmxheWVycy5zZXQodGhpcy5fdGhpcmRQZXJzb25Pbmx5TGF5ZXIpO1xuICAgICAgICB9XG4gICAgICB9XG4gICAgfSk7XG4gIH1cblxuICAvKipcbiAgICogSXQganVzdCBjaGVja3Mgd2hldGhlciB0aGUgbm9kZSBvciBpdHMgcGFyZW50IGlzIHRoZSBmaXJzdCBwZXJzb24gYm9uZSBvciBub3QuXG4gICAqIEBwYXJhbSBib25lIFRoZSB0YXJnZXQgYm9uZVxuICAgKi9cbiAgcHJpdmF0ZSBfaXNFcmFzZVRhcmdldChib25lOiBHTFRGTm9kZSk6IGJvb2xlYW4ge1xuICAgIGlmIChib25lID09PSB0aGlzLl9maXJzdFBlcnNvbkJvbmUpIHtcbiAgICAgIHJldHVybiB0cnVlO1xuICAgIH0gZWxzZSBpZiAoIWJvbmUucGFyZW50KSB7XG4gICAgICByZXR1cm4gZmFsc2U7XG4gICAgfSBlbHNlIHtcbiAgICAgIHJldHVybiB0aGlzLl9pc0VyYXNlVGFyZ2V0KGJvbmUucGFyZW50ISk7XG4gICAgfVxuICB9XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5pbXBvcnQgeyBHTFRGIH0gZnJvbSAndGhyZWUvZXhhbXBsZXMvanNtL2xvYWRlcnMvR0xURkxvYWRlcic7XG5pbXBvcnQgeyBWUk1IdW1hbm9pZCB9IGZyb20gJy4uL2h1bWFub2lkJztcbmltcG9ydCB7IEdMVEZOb2RlLCBHTFRGU2NoZW1hLCBWUk1TY2hlbWEgfSBmcm9tICcuLi90eXBlcyc7XG5pbXBvcnQgeyBnbHRmRXh0cmFjdFByaW1pdGl2ZXNGcm9tTm9kZXMgfSBmcm9tICcuLi91dGlscy9nbHRmRXh0cmFjdFByaW1pdGl2ZXNGcm9tTm9kZSc7XG5pbXBvcnQgeyBWUk1GaXJzdFBlcnNvbiwgVlJNUmVuZGVyZXJGaXJzdFBlcnNvbkZsYWdzIH0gZnJvbSAnLi9WUk1GaXJzdFBlcnNvbic7XG5cbi8qKlxuICogQW4gaW1wb3J0ZXIgdGhhdCBpbXBvcnRzIGEgW1tWUk1GaXJzdFBlcnNvbl1dIGZyb20gYSBWUk0gZXh0ZW5zaW9uIG9mIGEgR0xURi5cbiAqL1xuZXhwb3J0IGNsYXNzIFZSTUZpcnN0UGVyc29uSW1wb3J0ZXIge1xuICAvKipcbiAgICogSW1wb3J0IGEgW1tWUk1GaXJzdFBlcnNvbl1dIGZyb20gYSBWUk0uXG4gICAqXG4gICAqIEBwYXJhbSBnbHRmIEEgcGFyc2VkIHJlc3VsdCBvZiBHTFRGIHRha2VuIGZyb20gR0xURkxvYWRlclxuICAgKiBAcGFyYW0gaHVtYW5vaWQgQSBbW1ZSTUh1bWFub2lkXV0gaW5zdGFuY2UgdGhhdCByZXByZXNlbnRzIHRoZSBWUk1cbiAgICovXG4gIHB1YmxpYyBhc3luYyBpbXBvcnQoZ2x0ZjogR0xURiwgaHVtYW5vaWQ6IFZSTUh1bWFub2lkKTogUHJvbWlzZTxWUk1GaXJzdFBlcnNvbiB8IG51bGw+IHtcbiAgICBjb25zdCB2cm1FeHQ6IFZSTVNjaGVtYS5WUk0gfCB1bmRlZmluZWQgPSBnbHRmLnBhcnNlci5qc29uLmV4dGVuc2lvbnM/LlZSTTtcbiAgICBpZiAoIXZybUV4dCkge1xuICAgICAgcmV0dXJuIG51bGw7XG4gICAgfVxuXG4gICAgY29uc3Qgc2NoZW1hRmlyc3RQZXJzb246IFZSTVNjaGVtYS5GaXJzdFBlcnNvbiB8IHVuZGVmaW5lZCA9IHZybUV4dC5maXJzdFBlcnNvbjtcbiAgICBpZiAoIXNjaGVtYUZpcnN0UGVyc29uKSB7XG4gICAgICByZXR1cm4gbnVsbDtcbiAgICB9XG5cbiAgICBjb25zdCBmaXJzdFBlcnNvbkJvbmVJbmRleCA9IHNjaGVtYUZpcnN0UGVyc29uLmZpcnN0UGVyc29uQm9uZTtcblxuICAgIGxldCBmaXJzdFBlcnNvbkJvbmU6IEdMVEZOb2RlIHwgbnVsbDtcbiAgICBpZiAoZmlyc3RQZXJzb25Cb25lSW5kZXggPT09IHVuZGVmaW5lZCB8fCBmaXJzdFBlcnNvbkJvbmVJbmRleCA9PT0gLTEpIHtcbiAgICAgIGZpcnN0UGVyc29uQm9uZSA9IGh1bWFub2lkLmdldEJvbmVOb2RlKFZSTVNjaGVtYS5IdW1hbm9pZEJvbmVOYW1lLkhlYWQpO1xuICAgIH0gZWxzZSB7XG4gICAgICBmaXJzdFBlcnNvbkJvbmUgPSBhd2FpdCBnbHRmLnBhcnNlci5nZXREZXBlbmRlbmN5KCdub2RlJywgZmlyc3RQZXJzb25Cb25lSW5kZXgpO1xuICAgIH1cblxuICAgIGlmICghZmlyc3RQZXJzb25Cb25lKSB7XG4gICAgICBjb25zb2xlLndhcm4oJ1ZSTUZpcnN0UGVyc29uSW1wb3J0ZXI6IENvdWxkIG5vdCBmaW5kIGZpcnN0UGVyc29uQm9uZSBvZiB0aGUgVlJNJyk7XG4gICAgICByZXR1cm4gbnVsbDtcbiAgICB9XG5cbiAgICBjb25zdCBmaXJzdFBlcnNvbkJvbmVPZmZzZXQgPSBzY2hlbWFGaXJzdFBlcnNvbi5maXJzdFBlcnNvbkJvbmVPZmZzZXRcbiAgICAgID8gbmV3IFRIUkVFLlZlY3RvcjMoXG4gICAgICAgICAgc2NoZW1hRmlyc3RQZXJzb24uZmlyc3RQZXJzb25Cb25lT2Zmc2V0LngsXG4gICAgICAgICAgc2NoZW1hRmlyc3RQZXJzb24uZmlyc3RQZXJzb25Cb25lT2Zmc2V0LnksXG4gICAgICAgICAgLXNjaGVtYUZpcnN0UGVyc29uLmZpcnN0UGVyc29uQm9uZU9mZnNldC56ISwgLy8gVlJNIDAuMCB1c2VzIGxlZnQtaGFuZGVkIHktdXBcbiAgICAgICAgKVxuICAgICAgOiBuZXcgVEhSRUUuVmVjdG9yMygwLjAsIDAuMDYsIDAuMCk7IC8vIGZhbGxiYWNrLCB0YWtlbiBmcm9tIFVuaVZSTSBpbXBsZW1lbnRhdGlvblxuXG4gICAgY29uc3QgbWVzaEFubm90YXRpb25zOiBWUk1SZW5kZXJlckZpcnN0UGVyc29uRmxhZ3NbXSA9IFtdO1xuICAgIGNvbnN0IG5vZGVQcmltaXRpdmVzTWFwID0gYXdhaXQgZ2x0ZkV4dHJhY3RQcmltaXRpdmVzRnJvbU5vZGVzKGdsdGYpO1xuXG4gICAgQXJyYXkuZnJvbShub2RlUHJpbWl0aXZlc01hcC5lbnRyaWVzKCkpLmZvckVhY2goKFtub2RlSW5kZXgsIHByaW1pdGl2ZXNdKSA9PiB7XG4gICAgICBjb25zdCBzY2hlbWFOb2RlOiBHTFRGU2NoZW1hLk5vZGUgPSBnbHRmLnBhcnNlci5qc29uLm5vZGVzW25vZGVJbmRleF07XG5cbiAgICAgIGNvbnN0IGZsYWcgPSBzY2hlbWFGaXJzdFBlcnNvbi5tZXNoQW5ub3RhdGlvbnNcbiAgICAgICAgPyBzY2hlbWFGaXJzdFBlcnNvbi5tZXNoQW5ub3RhdGlvbnMuZmluZCgoYSkgPT4gYS5tZXNoID09PSBzY2hlbWFOb2RlLm1lc2gpXG4gICAgICAgIDogdW5kZWZpbmVkO1xuICAgICAgbWVzaEFubm90YXRpb25zLnB1c2gobmV3IFZSTVJlbmRlcmVyRmlyc3RQZXJzb25GbGFncyhmbGFnPy5maXJzdFBlcnNvbkZsYWcsIHByaW1pdGl2ZXMpKTtcbiAgICB9KTtcblxuICAgIHJldHVybiBuZXcgVlJNRmlyc3RQZXJzb24oZmlyc3RQZXJzb25Cb25lLCBmaXJzdFBlcnNvbkJvbmVPZmZzZXQsIG1lc2hBbm5vdGF0aW9ucyk7XG4gIH1cbn1cbiIsImltcG9ydCB7IEdMVEZOb2RlIH0gZnJvbSAnLi4vdHlwZXMnO1xuaW1wb3J0IHsgVlJNSHVtYW5MaW1pdCB9IGZyb20gJy4vVlJNSHVtYW5MaW1pdCc7XG5cbi8qKlxuICogQSBjbGFzcyByZXByZXNlbnRzIGEgc2luZ2xlIGBodW1hbkJvbmVgIG9mIGEgVlJNLlxuICovXG5leHBvcnQgY2xhc3MgVlJNSHVtYW5Cb25lIHtcbiAgLyoqXG4gICAqIEEgW1tHTFRGTm9kZV1dICh0aGF0IGFjdHVhbGx5IGlzIGEgYFRIUkVFLk9iamVjdDNEYCkgdGhhdCByZXByZXNlbnRzIHRoZSBib25lLlxuICAgKi9cbiAgcHVibGljIHJlYWRvbmx5IG5vZGU6IEdMVEZOb2RlO1xuXG4gIC8qKlxuICAgKiBBIFtbVlJNSHVtYW5MaW1pdF1dIG9iamVjdCB0aGF0IHJlcHJlc2VudHMgcHJvcGVydGllcyBvZiB0aGUgYm9uZS5cbiAgICovXG4gIHB1YmxpYyByZWFkb25seSBodW1hbkxpbWl0OiBWUk1IdW1hbkxpbWl0O1xuXG4gIC8qKlxuICAgKiBDcmVhdGUgYSBuZXcgVlJNSHVtYW5Cb25lLlxuICAgKlxuICAgKiBAcGFyYW0gbm9kZSBBIFtbR0xURk5vZGVdXSB0aGF0IHJlcHJlc2VudHMgdGhlIG5ldyBib25lXG4gICAqIEBwYXJhbSBodW1hbkxpbWl0IEEgW1tWUk1IdW1hbkxpbWl0XV0gb2JqZWN0IHRoYXQgcmVwcmVzZW50cyBwcm9wZXJ0aWVzIG9mIHRoZSBuZXcgYm9uZVxuICAgKi9cbiAgcHVibGljIGNvbnN0cnVjdG9yKG5vZGU6IEdMVEZOb2RlLCBodW1hbkxpbWl0OiBWUk1IdW1hbkxpbWl0KSB7XG4gICAgdGhpcy5ub2RlID0gbm9kZTtcbiAgICB0aGlzLmh1bWFuTGltaXQgPSBodW1hbkxpbWl0O1xuICB9XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5cbi8qKlxuICogQSBjb21wYXQgZnVuY3Rpb24gZm9yIGBRdWF0ZXJuaW9uLmludmVydCgpYCAvIGBRdWF0ZXJuaW9uLmludmVyc2UoKWAuXG4gKiBgUXVhdGVybmlvbi5pbnZlcnQoKWAgaXMgaW50cm9kdWNlZCBpbiByMTIzIGFuZCBgUXVhdGVybmlvbi5pbnZlcnNlKClgIGVtaXRzIGEgd2FybmluZy5cbiAqIFdlIGFyZSBnb2luZyB0byB1c2UgdGhpcyBjb21wYXQgZm9yIGEgd2hpbGUuXG4gKiBAcGFyYW0gdGFyZ2V0IEEgdGFyZ2V0IHF1YXRlcm5pb25cbiAqL1xuZXhwb3J0IGZ1bmN0aW9uIHF1YXRJbnZlcnRDb21wYXQ8VCBleHRlbmRzIFRIUkVFLlF1YXRlcm5pb24+KHRhcmdldDogVCk6IFQge1xuICBpZiAoKHRhcmdldCBhcyBhbnkpLmludmVydCkge1xuICAgIHRhcmdldC5pbnZlcnQoKTtcbiAgfSBlbHNlIHtcbiAgICAodGFyZ2V0IGFzIGFueSkuaW52ZXJzZSgpO1xuICB9XG5cbiAgcmV0dXJuIHRhcmdldDtcbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcbmltcG9ydCB7IEdMVEZOb2RlLCBSYXdWZWN0b3IzLCBSYXdWZWN0b3I0LCBWUk1Qb3NlLCBWUk1TY2hlbWEgfSBmcm9tICcuLi90eXBlcyc7XG5pbXBvcnQgeyBxdWF0SW52ZXJ0Q29tcGF0IH0gZnJvbSAnLi4vdXRpbHMvcXVhdEludmVydENvbXBhdCc7XG5pbXBvcnQgeyBWUk1IdW1hbkJvbmUgfSBmcm9tICcuL1ZSTUh1bWFuQm9uZSc7XG5pbXBvcnQgeyBWUk1IdW1hbkJvbmVBcnJheSB9IGZyb20gJy4vVlJNSHVtYW5Cb25lQXJyYXknO1xuaW1wb3J0IHsgVlJNSHVtYW5Cb25lcyB9IGZyb20gJy4vVlJNSHVtYW5Cb25lcyc7XG5pbXBvcnQgeyBWUk1IdW1hbkRlc2NyaXB0aW9uIH0gZnJvbSAnLi9WUk1IdW1hbkRlc2NyaXB0aW9uJztcblxuY29uc3QgX3YzQSA9IG5ldyBUSFJFRS5WZWN0b3IzKCk7XG5jb25zdCBfcXVhdEEgPSBuZXcgVEhSRUUuUXVhdGVybmlvbigpO1xuXG4vKipcbiAqIEEgY2xhc3MgcmVwcmVzZW50cyBodW1hbm9pZCBvZiBhIFZSTS5cbiAqL1xuZXhwb3J0IGNsYXNzIFZSTUh1bWFub2lkIHtcbiAgLyoqXG4gICAqIEEgW1tWUk1IdW1hbkJvbmVzXV0gdGhhdCBjb250YWlucyBhbGwgdGhlIGh1bWFuIGJvbmVzIG9mIHRoZSBWUk0uXG4gICAqIFlvdSBtaWdodCB3YW50IHRvIGdldCB0aGVzZSBib25lcyB1c2luZyBbW1ZSTUh1bWFub2lkLmdldEJvbmVdXS5cbiAgICovXG4gIHB1YmxpYyByZWFkb25seSBodW1hbkJvbmVzOiBWUk1IdW1hbkJvbmVzO1xuXG4gIC8qKlxuICAgKiBBIFtbVlJNSHVtYW5EZXNjcmlwdGlvbl1dIHRoYXQgcmVwcmVzZW50cyBwcm9wZXJ0aWVzIG9mIHRoZSBodW1hbm9pZC5cbiAgICovXG4gIHB1YmxpYyByZWFkb25seSBodW1hbkRlc2NyaXB0aW9uOiBWUk1IdW1hbkRlc2NyaXB0aW9uO1xuXG4gIC8qKlxuICAgKiBBIFtbVlJNUG9zZV1dIHRoYXQgaXMgaXRzIGRlZmF1bHQgc3RhdGUuXG4gICAqIE5vdGUgdGhhdCBpdCdzIG5vdCBjb21wYXRpYmxlIHdpdGggYHNldFBvc2VgIGFuZCBgZ2V0UG9zZWAsIHNpbmNlIGl0IGNvbnRhaW5zIG5vbi1yZWxhdGl2ZSB2YWx1ZXMgb2YgZWFjaCBsb2NhbCB0cmFuc2Zvcm1zLlxuICAgKi9cbiAgcHVibGljIHJlYWRvbmx5IHJlc3RQb3NlOiBWUk1Qb3NlID0ge307XG5cbiAgLyoqXG4gICAqIENyZWF0ZSBhIG5ldyBbW1ZSTUh1bWFub2lkXV0uXG4gICAqIEBwYXJhbSBib25lQXJyYXkgQSBbW1ZSTUh1bWFuQm9uZUFycmF5XV0gY29udGFpbnMgYWxsIHRoZSBib25lcyBvZiB0aGUgbmV3IGh1bWFub2lkXG4gICAqIEBwYXJhbSBodW1hbkRlc2NyaXB0aW9uIEEgW1tWUk1IdW1hbkRlc2NyaXB0aW9uXV0gdGhhdCByZXByZXNlbnRzIHByb3BlcnRpZXMgb2YgdGhlIG5ldyBodW1hbm9pZFxuICAgKi9cbiAgcHVibGljIGNvbnN0cnVjdG9yKGJvbmVBcnJheTogVlJNSHVtYW5Cb25lQXJyYXksIGh1bWFuRGVzY3JpcHRpb246IFZSTUh1bWFuRGVzY3JpcHRpb24pIHtcbiAgICB0aGlzLmh1bWFuQm9uZXMgPSB0aGlzLl9jcmVhdGVIdW1hbkJvbmVzKGJvbmVBcnJheSk7XG4gICAgdGhpcy5odW1hbkRlc2NyaXB0aW9uID0gaHVtYW5EZXNjcmlwdGlvbjtcblxuICAgIHRoaXMucmVzdFBvc2UgPSB0aGlzLmdldFBvc2UoKTtcbiAgfVxuXG4gIC8qKlxuICAgKiBSZXR1cm4gdGhlIGN1cnJlbnQgcG9zZSBvZiB0aGlzIGh1bWFub2lkIGFzIGEgW1tWUk1Qb3NlXV0uXG4gICAqXG4gICAqIEVhY2ggdHJhbnNmb3JtIGlzIGEgbG9jYWwgdHJhbnNmb3JtIHJlbGF0aXZlIGZyb20gcmVzdCBwb3NlIChULXBvc2UpLlxuICAgKi9cbiAgcHVibGljIGdldFBvc2UoKTogVlJNUG9zZSB7XG4gICAgY29uc3QgcG9zZTogVlJNUG9zZSA9IHt9O1xuICAgIE9iamVjdC5rZXlzKHRoaXMuaHVtYW5Cb25lcykuZm9yRWFjaCgodnJtQm9uZU5hbWUpID0+IHtcbiAgICAgIGNvbnN0IG5vZGUgPSB0aGlzLmdldEJvbmVOb2RlKHZybUJvbmVOYW1lIGFzIFZSTVNjaGVtYS5IdW1hbm9pZEJvbmVOYW1lKSE7XG5cbiAgICAgIC8vIElnbm9yZSB3aGVuIHRoZXJlIGFyZSBubyBib25lIG9uIHRoZSBWUk1IdW1hbm9pZFxuICAgICAgaWYgKCFub2RlKSB7XG4gICAgICAgIHJldHVybjtcbiAgICAgIH1cblxuICAgICAgLy8gV2hlbiB0aGVyZSBhcmUgdHdvIG9yIG1vcmUgYm9uZXMgaW4gYSBzYW1lIG5hbWUsIHdlIGFyZSBub3QgZ29pbmcgdG8gb3ZlcndyaXRlIGV4aXN0aW5nIG9uZVxuICAgICAgaWYgKHBvc2VbdnJtQm9uZU5hbWVdKSB7XG4gICAgICAgIHJldHVybjtcbiAgICAgIH1cblxuICAgICAgLy8gVGFrZSBhIGRpZmYgZnJvbSByZXN0UG9zZVxuICAgICAgLy8gbm90ZSB0aGF0IHJlc3RQb3NlIGFsc28gd2lsbCB1c2UgZ2V0UG9zZSB0byBpbml0aWFsaXplIGl0c2VsZlxuICAgICAgX3YzQS5zZXQoMCwgMCwgMCk7XG4gICAgICBfcXVhdEEuaWRlbnRpdHkoKTtcblxuICAgICAgY29uc3QgcmVzdFN0YXRlID0gdGhpcy5yZXN0UG9zZVt2cm1Cb25lTmFtZV07XG4gICAgICBpZiAocmVzdFN0YXRlPy5wb3NpdGlvbikge1xuICAgICAgICBfdjNBLmZyb21BcnJheShyZXN0U3RhdGUucG9zaXRpb24pLm5lZ2F0ZSgpO1xuICAgICAgfVxuICAgICAgaWYgKHJlc3RTdGF0ZT8ucm90YXRpb24pIHtcbiAgICAgICAgcXVhdEludmVydENvbXBhdChfcXVhdEEuZnJvbUFycmF5KHJlc3RTdGF0ZS5yb3RhdGlvbikpO1xuICAgICAgfVxuXG4gICAgICAvLyBHZXQgdGhlIHBvc2l0aW9uIC8gcm90YXRpb24gZnJvbSB0aGUgbm9kZVxuICAgICAgX3YzQS5hZGQobm9kZS5wb3NpdGlvbik7XG4gICAgICBfcXVhdEEucHJlbXVsdGlwbHkobm9kZS5xdWF0ZXJuaW9uKTtcblxuICAgICAgcG9zZVt2cm1Cb25lTmFtZV0gPSB7XG4gICAgICAgIHBvc2l0aW9uOiBfdjNBLnRvQXJyYXkoKSBhcyBSYXdWZWN0b3IzLFxuICAgICAgICByb3RhdGlvbjogX3F1YXRBLnRvQXJyYXkoKSBhcyBSYXdWZWN0b3I0LFxuICAgICAgfTtcbiAgICB9LCB7fSBhcyBWUk1Qb3NlKTtcbiAgICByZXR1cm4gcG9zZTtcbiAgfVxuXG4gIC8qKlxuICAgKiBMZXQgdGhlIGh1bWFub2lkIGRvIGEgc3BlY2lmaWVkIHBvc2UuXG4gICAqXG4gICAqIEVhY2ggdHJhbnNmb3JtIGhhdmUgdG8gYmUgYSBsb2NhbCB0cmFuc2Zvcm0gcmVsYXRpdmUgZnJvbSByZXN0IHBvc2UgKFQtcG9zZSkuXG4gICAqIFlvdSBjYW4gcGFzcyB3aGF0IHlvdSBnb3QgZnJvbSB7QGxpbmsgZ2V0UG9zZX0uXG4gICAqXG4gICAqIEBwYXJhbSBwb3NlT2JqZWN0IEEgW1tWUk1Qb3NlXV0gdGhhdCByZXByZXNlbnRzIGEgc2luZ2xlIHBvc2VcbiAgICovXG4gIHB1YmxpYyBzZXRQb3NlKHBvc2VPYmplY3Q6IFZSTVBvc2UpOiB2b2lkIHtcbiAgICBPYmplY3Qua2V5cyhwb3NlT2JqZWN0KS5mb3JFYWNoKChib25lTmFtZSkgPT4ge1xuICAgICAgY29uc3Qgc3RhdGUgPSBwb3NlT2JqZWN0W2JvbmVOYW1lXSE7XG4gICAgICBjb25zdCBub2RlID0gdGhpcy5nZXRCb25lTm9kZShib25lTmFtZSBhcyBWUk1TY2hlbWEuSHVtYW5vaWRCb25lTmFtZSk7XG5cbiAgICAgIC8vIElnbm9yZSB3aGVuIHRoZXJlIGFyZSBubyBib25lIHRoYXQgaXMgZGVmaW5lZCBpbiB0aGUgcG9zZSBvbiB0aGUgVlJNSHVtYW5vaWRcbiAgICAgIGlmICghbm9kZSkge1xuICAgICAgICByZXR1cm47XG4gICAgICB9XG5cbiAgICAgIGNvbnN0IHJlc3RTdGF0ZSA9IHRoaXMucmVzdFBvc2VbYm9uZU5hbWVdO1xuICAgICAgaWYgKCFyZXN0U3RhdGUpIHtcbiAgICAgICAgcmV0dXJuO1xuICAgICAgfVxuXG4gICAgICBpZiAoc3RhdGUucG9zaXRpb24pIHtcbiAgICAgICAgbm9kZS5wb3NpdGlvbi5mcm9tQXJyYXkoc3RhdGUucG9zaXRpb24pO1xuXG4gICAgICAgIGlmIChyZXN0U3RhdGUucG9zaXRpb24pIHtcbiAgICAgICAgICBub2RlLnBvc2l0aW9uLmFkZChfdjNBLmZyb21BcnJheShyZXN0U3RhdGUucG9zaXRpb24pKTtcbiAgICAgICAgfVxuICAgICAgfVxuXG4gICAgICBpZiAoc3RhdGUucm90YXRpb24pIHtcbiAgICAgICAgbm9kZS5xdWF0ZXJuaW9uLmZyb21BcnJheShzdGF0ZS5yb3RhdGlvbik7XG5cbiAgICAgICAgaWYgKHJlc3RTdGF0ZS5yb3RhdGlvbikge1xuICAgICAgICAgIG5vZGUucXVhdGVybmlvbi5tdWx0aXBseShfcXVhdEEuZnJvbUFycmF5KHJlc3RTdGF0ZS5yb3RhdGlvbikpO1xuICAgICAgICB9XG4gICAgICB9XG4gICAgfSk7XG4gIH1cblxuICAvKipcbiAgICogUmVzZXQgdGhlIGh1bWFub2lkIHRvIGl0cyByZXN0IHBvc2UuXG4gICAqL1xuICBwdWJsaWMgcmVzZXRQb3NlKCk6IHZvaWQge1xuICAgIHRoaXMuc2V0UG9zZSh7fSk7XG4gIH1cblxuICAvKipcbiAgICogUmV0dXJuIGEgYm9uZSBib3VuZCB0byBhIHNwZWNpZmllZCBbW0h1bWFuQm9uZV1dLCBhcyBhIFtbVlJNSHVtYW5Cb25lXV0uXG4gICAqXG4gICAqIFNlZSBhbHNvOiBbW1ZSTUh1bWFub2lkLmdldEJvbmVzXV1cbiAgICpcbiAgICogQHBhcmFtIG5hbWUgTmFtZSBvZiB0aGUgYm9uZSB5b3Ugd2FudFxuICAgKi9cbiAgcHVibGljIGdldEJvbmUobmFtZTogVlJNU2NoZW1hLkh1bWFub2lkQm9uZU5hbWUpOiBWUk1IdW1hbkJvbmUgfCB1bmRlZmluZWQge1xuICAgIHJldHVybiB0aGlzLmh1bWFuQm9uZXNbbmFtZV1bMF0gfHwgdW5kZWZpbmVkO1xuICB9XG5cbiAgLyoqXG4gICAqIFJldHVybiBib25lcyBib3VuZCB0byBhIHNwZWNpZmllZCBbW0h1bWFuQm9uZV1dLCBhcyBhbiBhcnJheSBvZiBbW1ZSTUh1bWFuQm9uZV1dLlxuICAgKlxuICAgKiBTZWUgYWxzbzogW1tWUk1IdW1hbm9pZC5nZXRCb25lXV1cbiAgICpcbiAgICogQHBhcmFtIG5hbWUgTmFtZSBvZiB0aGUgYm9uZSB5b3Ugd2FudFxuICAgKi9cbiAgcHVibGljIGdldEJvbmVzKG5hbWU6IFZSTVNjaGVtYS5IdW1hbm9pZEJvbmVOYW1lKTogVlJNSHVtYW5Cb25lW10ge1xuICAgIHJldHVybiB0aGlzLmh1bWFuQm9uZXNbbmFtZV07XG4gIH1cblxuICAvKipcbiAgICogUmV0dXJuIGEgYm9uZSBib3VuZCB0byBhIHNwZWNpZmllZCBbW0h1bWFuQm9uZV1dLCBhcyBhIFRIUkVFLk9iamVjdDNELlxuICAgKlxuICAgKiBTZWUgYWxzbzogW1tWUk1IdW1hbm9pZC5nZXRCb25lTm9kZXNdXVxuICAgKlxuICAgKiBAcGFyYW0gbmFtZSBOYW1lIG9mIHRoZSBib25lIHlvdSB3YW50XG4gICAqL1xuICBwdWJsaWMgZ2V0Qm9uZU5vZGUobmFtZTogVlJNU2NoZW1hLkh1bWFub2lkQm9uZU5hbWUpOiBHTFRGTm9kZSB8IG51bGwge1xuICAgIHJldHVybiB0aGlzLmh1bWFuQm9uZXNbbmFtZV1bMF0/Lm5vZGUgPz8gbnVsbDtcbiAgfVxuXG4gIC8qKlxuICAgKiBSZXR1cm4gYm9uZXMgYm91bmQgdG8gYSBzcGVjaWZpZWQgW1tIdW1hbkJvbmVdXSwgYXMgYW4gYXJyYXkgb2YgVEhSRUUuT2JqZWN0M0QuXG4gICAqXG4gICAqIFNlZSBhbHNvOiBbW1ZSTUh1bWFub2lkLmdldEJvbmVOb2RlXV1cbiAgICpcbiAgICogQHBhcmFtIG5hbWUgTmFtZSBvZiB0aGUgYm9uZSB5b3Ugd2FudFxuICAgKi9cbiAgcHVibGljIGdldEJvbmVOb2RlcyhuYW1lOiBWUk1TY2hlbWEuSHVtYW5vaWRCb25lTmFtZSk6IEdMVEZOb2RlW10ge1xuICAgIHJldHVybiB0aGlzLmh1bWFuQm9uZXNbbmFtZV0ubWFwKChib25lKSA9PiBib25lLm5vZGUpO1xuICB9XG5cbiAgLyoqXG4gICAqIFByZXBhcmUgYSBbW1ZSTUh1bWFuQm9uZXNdXSBmcm9tIGEgW1tWUk1IdW1hbkJvbmVBcnJheV1dLlxuICAgKi9cbiAgcHJpdmF0ZSBfY3JlYXRlSHVtYW5Cb25lcyhib25lQXJyYXk6IFZSTUh1bWFuQm9uZUFycmF5KTogVlJNSHVtYW5Cb25lcyB7XG4gICAgY29uc3QgYm9uZXM6IFZSTUh1bWFuQm9uZXMgPSBPYmplY3QudmFsdWVzKFZSTVNjaGVtYS5IdW1hbm9pZEJvbmVOYW1lKS5yZWR1Y2UoKGFjY3VtLCBuYW1lKSA9PiB7XG4gICAgICBhY2N1bVtuYW1lXSA9IFtdO1xuICAgICAgcmV0dXJuIGFjY3VtO1xuICAgIH0sIHt9IGFzIFBhcnRpYWw8VlJNSHVtYW5Cb25lcz4pIGFzIFZSTUh1bWFuQm9uZXM7XG5cbiAgICBib25lQXJyYXkuZm9yRWFjaCgoYm9uZSkgPT4ge1xuICAgICAgYm9uZXNbYm9uZS5uYW1lXS5wdXNoKGJvbmUuYm9uZSk7XG4gICAgfSk7XG5cbiAgICByZXR1cm4gYm9uZXM7XG4gIH1cbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcbmltcG9ydCB7IEdMVEYgfSBmcm9tICd0aHJlZS9leGFtcGxlcy9qc20vbG9hZGVycy9HTFRGTG9hZGVyJztcbmltcG9ydCB7IFZSTVNjaGVtYSB9IGZyb20gJy4uL3R5cGVzJztcbmltcG9ydCB7IFZSTUh1bWFuQm9uZSB9IGZyb20gJy4vVlJNSHVtYW5Cb25lJztcbmltcG9ydCB7IFZSTUh1bWFuQm9uZUFycmF5IH0gZnJvbSAnLi9WUk1IdW1hbkJvbmVBcnJheSc7XG5pbXBvcnQgeyBWUk1IdW1hbkRlc2NyaXB0aW9uIH0gZnJvbSAnLi9WUk1IdW1hbkRlc2NyaXB0aW9uJztcbmltcG9ydCB7IFZSTUh1bWFub2lkIH0gZnJvbSAnLi9WUk1IdW1hbm9pZCc7XG5cbi8qKlxuICogQW4gaW1wb3J0ZXIgdGhhdCBpbXBvcnRzIGEgW1tWUk1IdW1hbm9pZF1dIGZyb20gYSBWUk0gZXh0ZW5zaW9uIG9mIGEgR0xURi5cbiAqL1xuZXhwb3J0IGNsYXNzIFZSTUh1bWFub2lkSW1wb3J0ZXIge1xuICAvKipcbiAgICogSW1wb3J0IGEgW1tWUk1IdW1hbm9pZF1dIGZyb20gYSBWUk0uXG4gICAqXG4gICAqIEBwYXJhbSBnbHRmIEEgcGFyc2VkIHJlc3VsdCBvZiBHTFRGIHRha2VuIGZyb20gR0xURkxvYWRlclxuICAgKi9cbiAgcHVibGljIGFzeW5jIGltcG9ydChnbHRmOiBHTFRGKTogUHJvbWlzZTxWUk1IdW1hbm9pZCB8IG51bGw+IHtcbiAgICBjb25zdCB2cm1FeHQ6IFZSTVNjaGVtYS5WUk0gfCB1bmRlZmluZWQgPSBnbHRmLnBhcnNlci5qc29uLmV4dGVuc2lvbnM/LlZSTTtcbiAgICBpZiAoIXZybUV4dCkge1xuICAgICAgcmV0dXJuIG51bGw7XG4gICAgfVxuXG4gICAgY29uc3Qgc2NoZW1hSHVtYW5vaWQ6IFZSTVNjaGVtYS5IdW1hbm9pZCB8IHVuZGVmaW5lZCA9IHZybUV4dC5odW1hbm9pZDtcbiAgICBpZiAoIXNjaGVtYUh1bWFub2lkKSB7XG4gICAgICByZXR1cm4gbnVsbDtcbiAgICB9XG5cbiAgICBjb25zdCBodW1hbkJvbmVBcnJheTogVlJNSHVtYW5Cb25lQXJyYXkgPSBbXTtcbiAgICBpZiAoc2NoZW1hSHVtYW5vaWQuaHVtYW5Cb25lcykge1xuICAgICAgYXdhaXQgUHJvbWlzZS5hbGwoXG4gICAgICAgIHNjaGVtYUh1bWFub2lkLmh1bWFuQm9uZXMubWFwKGFzeW5jIChib25lKSA9PiB7XG4gICAgICAgICAgaWYgKCFib25lLmJvbmUgfHwgYm9uZS5ub2RlID09IG51bGwpIHtcbiAgICAgICAgICAgIHJldHVybjtcbiAgICAgICAgICB9XG5cbiAgICAgICAgICBjb25zdCBub2RlID0gYXdhaXQgZ2x0Zi5wYXJzZXIuZ2V0RGVwZW5kZW5jeSgnbm9kZScsIGJvbmUubm9kZSk7XG4gICAgICAgICAgaHVtYW5Cb25lQXJyYXkucHVzaCh7XG4gICAgICAgICAgICBuYW1lOiBib25lLmJvbmUsXG4gICAgICAgICAgICBib25lOiBuZXcgVlJNSHVtYW5Cb25lKG5vZGUsIHtcbiAgICAgICAgICAgICAgYXhpc0xlbmd0aDogYm9uZS5heGlzTGVuZ3RoLFxuICAgICAgICAgICAgICBjZW50ZXI6IGJvbmUuY2VudGVyICYmIG5ldyBUSFJFRS5WZWN0b3IzKGJvbmUuY2VudGVyLngsIGJvbmUuY2VudGVyLnksIGJvbmUuY2VudGVyLnopLFxuICAgICAgICAgICAgICBtYXg6IGJvbmUubWF4ICYmIG5ldyBUSFJFRS5WZWN0b3IzKGJvbmUubWF4LngsIGJvbmUubWF4LnksIGJvbmUubWF4LnopLFxuICAgICAgICAgICAgICBtaW46IGJvbmUubWluICYmIG5ldyBUSFJFRS5WZWN0b3IzKGJvbmUubWluLngsIGJvbmUubWluLnksIGJvbmUubWluLnopLFxuICAgICAgICAgICAgICB1c2VEZWZhdWx0VmFsdWVzOiBib25lLnVzZURlZmF1bHRWYWx1ZXMsXG4gICAgICAgICAgICB9KSxcbiAgICAgICAgICB9KTtcbiAgICAgICAgfSksXG4gICAgICApO1xuICAgIH1cblxuICAgIGNvbnN0IGh1bWFuRGVzY3JpcHRpb246IFZSTUh1bWFuRGVzY3JpcHRpb24gPSB7XG4gICAgICBhcm1TdHJldGNoOiBzY2hlbWFIdW1hbm9pZC5hcm1TdHJldGNoLFxuICAgICAgbGVnU3RyZXRjaDogc2NoZW1hSHVtYW5vaWQubGVnU3RyZXRjaCxcbiAgICAgIHVwcGVyQXJtVHdpc3Q6IHNjaGVtYUh1bWFub2lkLnVwcGVyQXJtVHdpc3QsXG4gICAgICBsb3dlckFybVR3aXN0OiBzY2hlbWFIdW1hbm9pZC5sb3dlckFybVR3aXN0LFxuICAgICAgdXBwZXJMZWdUd2lzdDogc2NoZW1hSHVtYW5vaWQudXBwZXJMZWdUd2lzdCxcbiAgICAgIGxvd2VyTGVnVHdpc3Q6IHNjaGVtYUh1bWFub2lkLmxvd2VyTGVnVHdpc3QsXG4gICAgICBmZWV0U3BhY2luZzogc2NoZW1hSHVtYW5vaWQuZmVldFNwYWNpbmcsXG4gICAgICBoYXNUcmFuc2xhdGlvbkRvRjogc2NoZW1hSHVtYW5vaWQuaGFzVHJhbnNsYXRpb25Eb0YsXG4gICAgfTtcblxuICAgIHJldHVybiBuZXcgVlJNSHVtYW5vaWQoaHVtYW5Cb25lQXJyYXksIGh1bWFuRGVzY3JpcHRpb24pO1xuICB9XG59XG4iLCIvKipcbiAqIEV2YWx1YXRlIGEgaGVybWl0ZSBzcGxpbmUuXG4gKlxuICogQHBhcmFtIHkwIHkgb24gc3RhcnRcbiAqIEBwYXJhbSB5MSB5IG9uIGVuZFxuICogQHBhcmFtIHQwIGRlbHRhIHkgb24gc3RhcnRcbiAqIEBwYXJhbSB0MSBkZWx0YSB5IG9uIGVuZFxuICogQHBhcmFtIHggaW5wdXQgdmFsdWVcbiAqL1xuY29uc3QgaGVybWl0ZVNwbGluZSA9ICh5MDogbnVtYmVyLCB5MTogbnVtYmVyLCB0MDogbnVtYmVyLCB0MTogbnVtYmVyLCB4OiBudW1iZXIpOiBudW1iZXIgPT4ge1xuICBjb25zdCB4YyA9IHggKiB4ICogeDtcbiAgY29uc3QgeHMgPSB4ICogeDtcbiAgY29uc3QgZHkgPSB5MSAtIHkwO1xuICBjb25zdCBoMDEgPSAtMi4wICogeGMgKyAzLjAgKiB4cztcbiAgY29uc3QgaDEwID0geGMgLSAyLjAgKiB4cyArIHg7XG4gIGNvbnN0IGgxMSA9IHhjIC0geHM7XG4gIHJldHVybiB5MCArIGR5ICogaDAxICsgdDAgKiBoMTAgKyB0MSAqIGgxMTtcbn07XG5cbi8qKlxuICogRXZhbHVhdGUgYW4gQW5pbWF0aW9uQ3VydmUgYXJyYXkuIFNlZSBBbmltYXRpb25DdXJ2ZSBjbGFzcyBvZiBVbml0eSBmb3IgaXRzIGRldGFpbHMuXG4gKlxuICogU2VlOiBodHRwczovL2RvY3MudW5pdHkzZC5jb20vamEvY3VycmVudC9TY3JpcHRSZWZlcmVuY2UvQW5pbWF0aW9uQ3VydmUuaHRtbFxuICpcbiAqIEBwYXJhbSBhcnIgQW4gYXJyYXkgcmVwcmVzZW50cyBhIGN1cnZlXG4gKiBAcGFyYW0geCBBbiBpbnB1dCB2YWx1ZVxuICovXG5jb25zdCBldmFsdWF0ZUN1cnZlID0gKGFycjogbnVtYmVyW10sIHg6IG51bWJlcik6IG51bWJlciA9PiB7XG4gIC8vIC0tIHNhbml0eSBjaGVjayAtLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLVxuICBpZiAoYXJyLmxlbmd0aCA8IDgpIHtcbiAgICB0aHJvdyBuZXcgRXJyb3IoJ2V2YWx1YXRlQ3VydmU6IEludmFsaWQgY3VydmUgZGV0ZWN0ZWQhIChBcnJheSBsZW5ndGggbXVzdCBiZSA4IGF0IGxlYXN0KScpO1xuICB9XG4gIGlmIChhcnIubGVuZ3RoICUgNCAhPT0gMCkge1xuICAgIHRocm93IG5ldyBFcnJvcignZXZhbHVhdGVDdXJ2ZTogSW52YWxpZCBjdXJ2ZSBkZXRlY3RlZCEgKEFycmF5IGxlbmd0aCBtdXN0IGJlIG11bHRpcGxlcyBvZiA0Jyk7XG4gIH1cblxuICAvLyAtLSBjaGVjayByYW5nZSAtLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS1cbiAgbGV0IG91dE5vZGU7XG4gIGZvciAob3V0Tm9kZSA9IDA7IDsgb3V0Tm9kZSsrKSB7XG4gICAgaWYgKGFyci5sZW5ndGggPD0gNCAqIG91dE5vZGUpIHtcbiAgICAgIHJldHVybiBhcnJbNCAqIG91dE5vZGUgLSAzXTsgLy8gdG9vIGZ1cnRoZXIhISBhc3N1bWUgYXMgXCJDbGFtcFwiXG4gICAgfSBlbHNlIGlmICh4IDw9IGFycls0ICogb3V0Tm9kZV0pIHtcbiAgICAgIGJyZWFrO1xuICAgIH1cbiAgfVxuXG4gIGNvbnN0IGluTm9kZSA9IG91dE5vZGUgLSAxO1xuICBpZiAoaW5Ob2RlIDwgMCkge1xuICAgIHJldHVybiBhcnJbNCAqIGluTm9kZSArIDVdOyAvLyB0b28gYmVoaW5kISEgYXNzdW1lIGFzIFwiQ2xhbXBcIlxuICB9XG5cbiAgLy8gLS0gY2FsY3VsYXRlIGxvY2FsIHggLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tXG4gIGNvbnN0IHgwID0gYXJyWzQgKiBpbk5vZGVdO1xuICBjb25zdCB4MSA9IGFycls0ICogb3V0Tm9kZV07XG4gIGNvbnN0IHhIZXJtaXRlID0gKHggLSB4MCkgLyAoeDEgLSB4MCk7XG5cbiAgLy8gLS0gZmluYWxseSBkbyB0aGUgaGVybWl0ZSBzcGxpbmUgLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tXG4gIGNvbnN0IHkwID0gYXJyWzQgKiBpbk5vZGUgKyAxXTtcbiAgY29uc3QgeTEgPSBhcnJbNCAqIG91dE5vZGUgKyAxXTtcbiAgY29uc3QgdDAgPSBhcnJbNCAqIGluTm9kZSArIDNdO1xuICBjb25zdCB0MSA9IGFycls0ICogb3V0Tm9kZSArIDJdO1xuICByZXR1cm4gaGVybWl0ZVNwbGluZSh5MCwgeTEsIHQwLCB0MSwgeEhlcm1pdGUpO1xufTtcblxuLyoqXG4gKiBUaGlzIGlzIGFuIGVxdWl2YWxlbnQgb2YgQ3VydmVNYXBwZXIgY2xhc3MgZGVmaW5lZCBpbiBVbmlWUk0uXG4gKiBXaWxsIGJlIHVzZWQgZm9yIFtbVlJNTG9va0F0QXBwbHllcl1dcywgdG8gZGVmaW5lIGJlaGF2aW9yIG9mIExvb2tBdC5cbiAqXG4gKiBTZWU6IGh0dHBzOi8vZ2l0aHViLmNvbS92cm0tYy9VbmlWUk0vYmxvYi9tYXN0ZXIvQXNzZXRzL1ZSTS9VbmlWUk0vU2NyaXB0cy9Mb29rQXQvQ3VydmVNYXBwZXIuY3NcbiAqL1xuZXhwb3J0IGNsYXNzIFZSTUN1cnZlTWFwcGVyIHtcbiAgLyoqXG4gICAqIEFuIGFycmF5IHJlcHJlc2VudHMgdGhlIGN1cnZlLiBTZWUgQW5pbWF0aW9uQ3VydmUgY2xhc3Mgb2YgVW5pdHkgZm9yIGl0cyBkZXRhaWxzLlxuICAgKlxuICAgKiBTZWU6IGh0dHBzOi8vZG9jcy51bml0eTNkLmNvbS9qYS9jdXJyZW50L1NjcmlwdFJlZmVyZW5jZS9BbmltYXRpb25DdXJ2ZS5odG1sXG4gICAqL1xuICBwdWJsaWMgY3VydmU6IG51bWJlcltdID0gWzAuMCwgMC4wLCAwLjAsIDEuMCwgMS4wLCAxLjAsIDEuMCwgMC4wXTtcblxuICAvKipcbiAgICogVGhlIG1heGltdW0gaW5wdXQgcmFuZ2Ugb2YgdGhlIFtbVlJNQ3VydmVNYXBwZXJdXS5cbiAgICovXG4gIHB1YmxpYyBjdXJ2ZVhSYW5nZURlZ3JlZSA9IDkwLjA7XG5cbiAgLyoqXG4gICAqIFRoZSBtYXhpbXVtIG91dHB1dCB2YWx1ZSBvZiB0aGUgW1tWUk1DdXJ2ZU1hcHBlcl1dLlxuICAgKi9cbiAgcHVibGljIGN1cnZlWVJhbmdlRGVncmVlID0gMTAuMDtcblxuICAvKipcbiAgICogQ3JlYXRlIGEgbmV3IFtbVlJNQ3VydmVNYXBwZXJdXS5cbiAgICpcbiAgICogQHBhcmFtIHhSYW5nZSBUaGUgbWF4aW11bSBpbnB1dCByYW5nZVxuICAgKiBAcGFyYW0geVJhbmdlIFRoZSBtYXhpbXVtIG91dHB1dCB2YWx1ZVxuICAgKiBAcGFyYW0gY3VydmUgQW4gYXJyYXkgcmVwcmVzZW50cyB0aGUgY3VydmVcbiAgICovXG4gIGNvbnN0cnVjdG9yKHhSYW5nZT86IG51bWJlciwgeVJhbmdlPzogbnVtYmVyLCBjdXJ2ZT86IG51bWJlcltdKSB7XG4gICAgaWYgKHhSYW5nZSAhPT0gdW5kZWZpbmVkKSB7XG4gICAgICB0aGlzLmN1cnZlWFJhbmdlRGVncmVlID0geFJhbmdlO1xuICAgIH1cblxuICAgIGlmICh5UmFuZ2UgIT09IHVuZGVmaW5lZCkge1xuICAgICAgdGhpcy5jdXJ2ZVlSYW5nZURlZ3JlZSA9IHlSYW5nZTtcbiAgICB9XG5cbiAgICBpZiAoY3VydmUgIT09IHVuZGVmaW5lZCkge1xuICAgICAgdGhpcy5jdXJ2ZSA9IGN1cnZlO1xuICAgIH1cbiAgfVxuXG4gIC8qKlxuICAgKiBFdmFsdWF0ZSBhbiBpbnB1dCB2YWx1ZSBhbmQgb3V0cHV0IGEgbWFwcGVkIHZhbHVlLlxuICAgKlxuICAgKiBAcGFyYW0gc3JjIFRoZSBpbnB1dCB2YWx1ZVxuICAgKi9cbiAgcHVibGljIG1hcChzcmM6IG51bWJlcik6IG51bWJlciB7XG4gICAgY29uc3QgY2xhbXBlZFNyYyA9IE1hdGgubWluKE1hdGgubWF4KHNyYywgMC4wKSwgdGhpcy5jdXJ2ZVhSYW5nZURlZ3JlZSk7XG4gICAgY29uc3QgeCA9IGNsYW1wZWRTcmMgLyB0aGlzLmN1cnZlWFJhbmdlRGVncmVlO1xuICAgIHJldHVybiB0aGlzLmN1cnZlWVJhbmdlRGVncmVlICogZXZhbHVhdGVDdXJ2ZSh0aGlzLmN1cnZlLCB4KTtcbiAgfVxufVxuIiwiaW1wb3J0ICogYXMgVEhSRUUgZnJvbSAndGhyZWUnO1xuaW1wb3J0IHsgVlJNU2NoZW1hIH0gZnJvbSAnLi4vdHlwZXMnO1xuXG4vKipcbiAqIFRoaXMgY2xhc3MgaXMgdXNlZCBieSBbW1ZSTUxvb2tBdEhlYWRdXSwgYXBwbGllcyBsb29rIGF0IGRpcmVjdGlvbi5cbiAqIFRoZXJlIGFyZSBjdXJyZW50bHkgdHdvIHZhcmlhbnQgb2YgYXBwbGllcjogW1tWUk1Mb29rQXRCb25lQXBwbHllcl1dIGFuZCBbW1ZSTUxvb2tBdEJsZW5kU2hhcGVBcHBseWVyXV0uXG4gKi9cbmV4cG9ydCBhYnN0cmFjdCBjbGFzcyBWUk1Mb29rQXRBcHBseWVyIHtcbiAgLyoqXG4gICAqIEl0IHJlcHJlc2VudHMgaXRzIHR5cGUgb2YgYXBwbGllci5cbiAgICovXG4gIHB1YmxpYyBhYnN0cmFjdCByZWFkb25seSB0eXBlOiBWUk1TY2hlbWEuRmlyc3RQZXJzb25Mb29rQXRUeXBlTmFtZTtcblxuICAvKipcbiAgICogQXBwbHkgbG9vayBhdCBkaXJlY3Rpb24gdG8gaXRzIGFzc29jaWF0ZWQgVlJNIG1vZGVsLlxuICAgKlxuICAgKiBAcGFyYW0gZXVsZXIgYFRIUkVFLkV1bGVyYCBvYmplY3QgdGhhdCByZXByZXNlbnRzIHRoZSBsb29rIGF0IGRpcmVjdGlvblxuICAgKi9cbiAgcHVibGljIGFic3RyYWN0IGxvb2tBdChldWxlcjogVEhSRUUuRXVsZXIpOiB2b2lkO1xufVxuIiwiaW1wb3J0ICogYXMgVEhSRUUgZnJvbSAndGhyZWUnO1xuaW1wb3J0IHsgVlJNQmxlbmRTaGFwZVByb3h5IH0gZnJvbSAnLi4vYmxlbmRzaGFwZSc7XG5pbXBvcnQgeyBWUk1TY2hlbWEgfSBmcm9tICcuLi90eXBlcyc7XG5pbXBvcnQgeyBWUk1DdXJ2ZU1hcHBlciB9IGZyb20gJy4vVlJNQ3VydmVNYXBwZXInO1xuaW1wb3J0IHsgVlJNTG9va0F0QXBwbHllciB9IGZyb20gJy4vVlJNTG9va0F0QXBwbHllcic7XG5cbi8qKlxuICogVGhpcyBjbGFzcyBpcyB1c2VkIGJ5IFtbVlJNTG9va0F0SGVhZF1dLCBhcHBsaWVzIGxvb2sgYXQgZGlyZWN0aW9uIHRvIGV5ZSBibGVuZCBzaGFwZXMgb2YgYSBWUk0uXG4gKi9cbmV4cG9ydCBjbGFzcyBWUk1Mb29rQXRCbGVuZFNoYXBlQXBwbHllciBleHRlbmRzIFZSTUxvb2tBdEFwcGx5ZXIge1xuICBwdWJsaWMgcmVhZG9ubHkgdHlwZSA9IFZSTVNjaGVtYS5GaXJzdFBlcnNvbkxvb2tBdFR5cGVOYW1lLkJsZW5kU2hhcGU7XG5cbiAgcHJpdmF0ZSByZWFkb25seSBfY3VydmVIb3Jpem9udGFsOiBWUk1DdXJ2ZU1hcHBlcjtcbiAgcHJpdmF0ZSByZWFkb25seSBfY3VydmVWZXJ0aWNhbERvd246IFZSTUN1cnZlTWFwcGVyO1xuICBwcml2YXRlIHJlYWRvbmx5IF9jdXJ2ZVZlcnRpY2FsVXA6IFZSTUN1cnZlTWFwcGVyO1xuXG4gIHByaXZhdGUgcmVhZG9ubHkgX2JsZW5kU2hhcGVQcm94eTogVlJNQmxlbmRTaGFwZVByb3h5O1xuXG4gIC8qKlxuICAgKiBDcmVhdGUgYSBuZXcgVlJNTG9va0F0QmxlbmRTaGFwZUFwcGx5ZXIuXG4gICAqXG4gICAqIEBwYXJhbSBibGVuZFNoYXBlUHJveHkgQSBbW1ZSTUJsZW5kU2hhcGVQcm94eV1dIHVzZWQgYnkgdGhpcyBhcHBsaWVyXG4gICAqIEBwYXJhbSBjdXJ2ZUhvcml6b250YWwgQSBbW1ZSTUN1cnZlTWFwcGVyXV0gdXNlZCBmb3IgdHJhbnN2ZXJzZSBkaXJlY3Rpb25cbiAgICogQHBhcmFtIGN1cnZlVmVydGljYWxEb3duIEEgW1tWUk1DdXJ2ZU1hcHBlcl1dIHVzZWQgZm9yIGRvd24gZGlyZWN0aW9uXG4gICAqIEBwYXJhbSBjdXJ2ZVZlcnRpY2FsVXAgQSBbW1ZSTUN1cnZlTWFwcGVyXV0gdXNlZCBmb3IgdXAgZGlyZWN0aW9uXG4gICAqL1xuICBjb25zdHJ1Y3RvcihcbiAgICBibGVuZFNoYXBlUHJveHk6IFZSTUJsZW5kU2hhcGVQcm94eSxcbiAgICBjdXJ2ZUhvcml6b250YWw6IFZSTUN1cnZlTWFwcGVyLFxuICAgIGN1cnZlVmVydGljYWxEb3duOiBWUk1DdXJ2ZU1hcHBlcixcbiAgICBjdXJ2ZVZlcnRpY2FsVXA6IFZSTUN1cnZlTWFwcGVyLFxuICApIHtcbiAgICBzdXBlcigpO1xuXG4gICAgdGhpcy5fY3VydmVIb3Jpem9udGFsID0gY3VydmVIb3Jpem9udGFsO1xuICAgIHRoaXMuX2N1cnZlVmVydGljYWxEb3duID0gY3VydmVWZXJ0aWNhbERvd247XG4gICAgdGhpcy5fY3VydmVWZXJ0aWNhbFVwID0gY3VydmVWZXJ0aWNhbFVwO1xuXG4gICAgdGhpcy5fYmxlbmRTaGFwZVByb3h5ID0gYmxlbmRTaGFwZVByb3h5O1xuICB9XG5cbiAgcHVibGljIG5hbWUoKTogVlJNU2NoZW1hLkZpcnN0UGVyc29uTG9va0F0VHlwZU5hbWUge1xuICAgIHJldHVybiBWUk1TY2hlbWEuRmlyc3RQZXJzb25Mb29rQXRUeXBlTmFtZS5CbGVuZFNoYXBlO1xuICB9XG5cbiAgcHVibGljIGxvb2tBdChldWxlcjogVEhSRUUuRXVsZXIpOiB2b2lkIHtcbiAgICBjb25zdCBzcmNYID0gZXVsZXIueDtcbiAgICBjb25zdCBzcmNZID0gZXVsZXIueTtcblxuICAgIGlmIChzcmNYIDwgMC4wKSB7XG4gICAgICB0aGlzLl9ibGVuZFNoYXBlUHJveHkuc2V0VmFsdWUoVlJNU2NoZW1hLkJsZW5kU2hhcGVQcmVzZXROYW1lLkxvb2t1cCwgMC4wKTtcbiAgICAgIHRoaXMuX2JsZW5kU2hhcGVQcm94eS5zZXRWYWx1ZShWUk1TY2hlbWEuQmxlbmRTaGFwZVByZXNldE5hbWUuTG9va2Rvd24sIHRoaXMuX2N1cnZlVmVydGljYWxEb3duLm1hcCgtc3JjWCkpO1xuICAgIH0gZWxzZSB7XG4gICAgICB0aGlzLl9ibGVuZFNoYXBlUHJveHkuc2V0VmFsdWUoVlJNU2NoZW1hLkJsZW5kU2hhcGVQcmVzZXROYW1lLkxvb2tkb3duLCAwLjApO1xuICAgICAgdGhpcy5fYmxlbmRTaGFwZVByb3h5LnNldFZhbHVlKFZSTVNjaGVtYS5CbGVuZFNoYXBlUHJlc2V0TmFtZS5Mb29rdXAsIHRoaXMuX2N1cnZlVmVydGljYWxVcC5tYXAoc3JjWCkpO1xuICAgIH1cblxuICAgIGlmIChzcmNZIDwgMC4wKSB7XG4gICAgICB0aGlzLl9ibGVuZFNoYXBlUHJveHkuc2V0VmFsdWUoVlJNU2NoZW1hLkJsZW5kU2hhcGVQcmVzZXROYW1lLkxvb2tsZWZ0LCAwLjApO1xuICAgICAgdGhpcy5fYmxlbmRTaGFwZVByb3h5LnNldFZhbHVlKFZSTVNjaGVtYS5CbGVuZFNoYXBlUHJlc2V0TmFtZS5Mb29rcmlnaHQsIHRoaXMuX2N1cnZlSG9yaXpvbnRhbC5tYXAoLXNyY1kpKTtcbiAgICB9IGVsc2Uge1xuICAgICAgdGhpcy5fYmxlbmRTaGFwZVByb3h5LnNldFZhbHVlKFZSTVNjaGVtYS5CbGVuZFNoYXBlUHJlc2V0TmFtZS5Mb29rcmlnaHQsIDAuMCk7XG4gICAgICB0aGlzLl9ibGVuZFNoYXBlUHJveHkuc2V0VmFsdWUoVlJNU2NoZW1hLkJsZW5kU2hhcGVQcmVzZXROYW1lLkxvb2tsZWZ0LCB0aGlzLl9jdXJ2ZUhvcml6b250YWwubWFwKHNyY1kpKTtcbiAgICB9XG4gIH1cbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcbmltcG9ydCB7IFZSTUZpcnN0UGVyc29uIH0gZnJvbSAnLi4vZmlyc3RwZXJzb24vVlJNRmlyc3RQZXJzb24nO1xuaW1wb3J0IHsgZ2V0V29ybGRRdWF0ZXJuaW9uTGl0ZSB9IGZyb20gJy4uL3V0aWxzL21hdGgnO1xuaW1wb3J0IHsgcXVhdEludmVydENvbXBhdCB9IGZyb20gJy4uL3V0aWxzL3F1YXRJbnZlcnRDb21wYXQnO1xuaW1wb3J0IHsgVlJNTG9va0F0QXBwbHllciB9IGZyb20gJy4vVlJNTG9va0F0QXBwbHllcic7XG5cbmNvbnN0IFZFQ1RPUjNfRlJPTlQgPSBPYmplY3QuZnJlZXplKG5ldyBUSFJFRS5WZWN0b3IzKDAuMCwgMC4wLCAtMS4wKSk7XG5cbmNvbnN0IF92M0EgPSBuZXcgVEhSRUUuVmVjdG9yMygpO1xuY29uc3QgX3YzQiA9IG5ldyBUSFJFRS5WZWN0b3IzKCk7XG5jb25zdCBfdjNDID0gbmV3IFRIUkVFLlZlY3RvcjMoKTtcbmNvbnN0IF9xdWF0ID0gbmV3IFRIUkVFLlF1YXRlcm5pb24oKTtcblxuLyoqXG4gKiBBIGNsYXNzIHJlcHJlc2VudHMgbG9vayBhdCBvZiBhIFZSTS5cbiAqL1xuZXhwb3J0IGNsYXNzIFZSTUxvb2tBdEhlYWQge1xuICBwdWJsaWMgc3RhdGljIHJlYWRvbmx5IEVVTEVSX09SREVSID0gJ1lYWic7IC8vIHlhdy1waXRjaC1yb2xsXG5cbiAgLyoqXG4gICAqIEFzc29jaWF0ZWQgW1tWUk1GaXJzdFBlcnNvbl1dLCB3aWxsIGJlIHVzZWQgZm9yIGRpcmVjdGlvbiBjYWxjdWxhdGlvbi5cbiAgICovXG4gIHB1YmxpYyByZWFkb25seSBmaXJzdFBlcnNvbjogVlJNRmlyc3RQZXJzb247XG5cbiAgLyoqXG4gICAqIEFzc29jaWF0ZWQgW1tWUk1Mb29rQXRBcHBseWVyXV0sIGl0cyBsb29rIGF0IGRpcmVjdGlvbiB3aWxsIGJlIGFwcGxpZWQgdG8gdGhlIG1vZGVsIHVzaW5nIHRoaXMgYXBwbGllci5cbiAgICovXG4gIHB1YmxpYyByZWFkb25seSBhcHBseWVyPzogVlJNTG9va0F0QXBwbHllcjtcblxuICAvKipcbiAgICogSWYgdGhpcyBpcyB0cnVlLCBpdHMgbG9vayBhdCBkaXJlY3Rpb24gd2lsbCBiZSB1cGRhdGVkIGF1dG9tYXRpY2FsbHkgYnkgY2FsbGluZyBbW1ZSTUxvb2tBdEhlYWQudXBkYXRlXV0gKHdoaWNoIGlzIGNhbGxlZCBmcm9tIFtbVlJNLnVwZGF0ZV1dKS5cbiAgICpcbiAgICogU2VlIGFsc286IFtbVlJNTG9va0F0SGVhZC50YXJnZXRdXVxuICAgKi9cbiAgcHVibGljIGF1dG9VcGRhdGUgPSB0cnVlO1xuXG4gIC8qKlxuICAgKiBUaGUgdGFyZ2V0IG9iamVjdCBvZiB0aGUgbG9vayBhdC5cbiAgICogTm90ZSB0aGF0IGl0IGRvZXMgbm90IG1ha2UgYW55IHNlbnNlIGlmIFtbVlJNTG9va0F0SGVhZC5hdXRvVXBkYXRlXV0gaXMgZGlzYWJsZWQuXG4gICAqL1xuICBwdWJsaWMgdGFyZ2V0PzogVEhSRUUuT2JqZWN0M0Q7XG5cbiAgcHJvdGVjdGVkIF9ldWxlcjogVEhSRUUuRXVsZXIgPSBuZXcgVEhSRUUuRXVsZXIoMC4wLCAwLjAsIDAuMCwgVlJNTG9va0F0SGVhZC5FVUxFUl9PUkRFUik7XG5cbiAgLyoqXG4gICAqIENyZWF0ZSBhIG5ldyBWUk1Mb29rQXRIZWFkLlxuICAgKlxuICAgKiBAcGFyYW0gZmlyc3RQZXJzb24gQSBbW1ZSTUZpcnN0UGVyc29uXV0gdGhhdCB3aWxsIGJlIGFzc29jaWF0ZWQgd2l0aCB0aGlzIG5ldyBWUk1Mb29rQXRIZWFkXG4gICAqIEBwYXJhbSBhcHBseWVyIEEgW1tWUk1Mb29rQXRBcHBseWVyXV0gdGhhdCB3aWxsIGJlIGFzc29jaWF0ZWQgd2l0aCB0aGlzIG5ldyBWUk1Mb29rQXRIZWFkXG4gICAqL1xuICBjb25zdHJ1Y3RvcihmaXJzdFBlcnNvbjogVlJNRmlyc3RQZXJzb24sIGFwcGx5ZXI/OiBWUk1Mb29rQXRBcHBseWVyKSB7XG4gICAgdGhpcy5maXJzdFBlcnNvbiA9IGZpcnN0UGVyc29uO1xuICAgIHRoaXMuYXBwbHllciA9IGFwcGx5ZXI7XG4gIH1cblxuICAvKipcbiAgICogR2V0IGl0cyBsb29rIGF0IGRpcmVjdGlvbiBpbiB3b3JsZCBjb29yZGluYXRlLlxuICAgKlxuICAgKiBAcGFyYW0gdGFyZ2V0IEEgdGFyZ2V0IGBUSFJFRS5WZWN0b3IzYFxuICAgKi9cbiAgcHVibGljIGdldExvb2tBdFdvcmxkRGlyZWN0aW9uKHRhcmdldDogVEhSRUUuVmVjdG9yMyk6IFRIUkVFLlZlY3RvcjMge1xuICAgIGNvbnN0IHJvdCA9IGdldFdvcmxkUXVhdGVybmlvbkxpdGUodGhpcy5maXJzdFBlcnNvbi5maXJzdFBlcnNvbkJvbmUsIF9xdWF0KTtcbiAgICByZXR1cm4gdGFyZ2V0LmNvcHkoVkVDVE9SM19GUk9OVCkuYXBwbHlFdWxlcih0aGlzLl9ldWxlcikuYXBwbHlRdWF0ZXJuaW9uKHJvdCk7XG4gIH1cblxuICAvKipcbiAgICogU2V0IGl0cyBsb29rIGF0IHBvc2l0aW9uLlxuICAgKiBOb3RlIHRoYXQgaXRzIHJlc3VsdCB3aWxsIGJlIGluc3RhbnRseSBvdmVyd3JpdHRlbiBpZiBbW1ZSTUxvb2tBdEhlYWQuYXV0b1VwZGF0ZV1dIGlzIGVuYWJsZWQuXG4gICAqXG4gICAqIEBwYXJhbSBwb3NpdGlvbiBBIHRhcmdldCBwb3NpdGlvblxuICAgKi9cbiAgcHVibGljIGxvb2tBdChwb3NpdGlvbjogVEhSRUUuVmVjdG9yMyk6IHZvaWQge1xuICAgIHRoaXMuX2NhbGNFdWxlcih0aGlzLl9ldWxlciwgcG9zaXRpb24pO1xuXG4gICAgaWYgKHRoaXMuYXBwbHllcikge1xuICAgICAgdGhpcy5hcHBseWVyLmxvb2tBdCh0aGlzLl9ldWxlcik7XG4gICAgfVxuICB9XG5cbiAgLyoqXG4gICAqIFVwZGF0ZSB0aGUgVlJNTG9va0F0SGVhZC5cbiAgICogSWYgW1tWUk1Mb29rQXRIZWFkLmF1dG9VcGRhdGVdXSBpcyBkaXNhYmxlZCwgaXQgd2lsbCBkbyBub3RoaW5nLlxuICAgKlxuICAgKiBAcGFyYW0gZGVsdGEgZGVsdGFUaW1lXG4gICAqL1xuICBwdWJsaWMgdXBkYXRlKGRlbHRhOiBudW1iZXIpOiB2b2lkIHtcbiAgICBpZiAodGhpcy50YXJnZXQgJiYgdGhpcy5hdXRvVXBkYXRlKSB7XG4gICAgICB0aGlzLmxvb2tBdCh0aGlzLnRhcmdldC5nZXRXb3JsZFBvc2l0aW9uKF92M0EpKTtcblxuICAgICAgaWYgKHRoaXMuYXBwbHllcikge1xuICAgICAgICB0aGlzLmFwcGx5ZXIubG9va0F0KHRoaXMuX2V1bGVyKTtcbiAgICAgIH1cbiAgICB9XG4gIH1cblxuICBwcm90ZWN0ZWQgX2NhbGNFdWxlcih0YXJnZXQ6IFRIUkVFLkV1bGVyLCBwb3NpdGlvbjogVEhSRUUuVmVjdG9yMyk6IFRIUkVFLkV1bGVyIHtcbiAgICBjb25zdCBoZWFkUG9zaXRpb24gPSB0aGlzLmZpcnN0UGVyc29uLmdldEZpcnN0UGVyc29uV29ybGRQb3NpdGlvbihfdjNCKTtcblxuICAgIC8vIExvb2sgYXQgZGlyZWN0aW9uIGluIHdvcmxkIGNvb3JkaW5hdGVcbiAgICBjb25zdCBsb29rQXREaXIgPSBfdjNDLmNvcHkocG9zaXRpb24pLnN1YihoZWFkUG9zaXRpb24pLm5vcm1hbGl6ZSgpO1xuXG4gICAgLy8gVHJhbnNmb3JtIHRoZSBkaXJlY3Rpb24gaW50byBsb2NhbCBjb29yZGluYXRlIGZyb20gdGhlIGZpcnN0IHBlcnNvbiBib25lXG4gICAgbG9va0F0RGlyLmFwcGx5UXVhdGVybmlvbihxdWF0SW52ZXJ0Q29tcGF0KGdldFdvcmxkUXVhdGVybmlvbkxpdGUodGhpcy5maXJzdFBlcnNvbi5maXJzdFBlcnNvbkJvbmUsIF9xdWF0KSkpO1xuXG4gICAgLy8gY29udmVydCB0aGUgZGlyZWN0aW9uIGludG8gZXVsZXJcbiAgICB0YXJnZXQueCA9IE1hdGguYXRhbjIobG9va0F0RGlyLnksIE1hdGguc3FydChsb29rQXREaXIueCAqIGxvb2tBdERpci54ICsgbG9va0F0RGlyLnogKiBsb29rQXREaXIueikpO1xuICAgIHRhcmdldC55ID0gTWF0aC5hdGFuMigtbG9va0F0RGlyLngsIC1sb29rQXREaXIueik7XG5cbiAgICByZXR1cm4gdGFyZ2V0O1xuICB9XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5pbXBvcnQgeyBWUk1IdW1hbm9pZCB9IGZyb20gJy4uL2h1bWFub2lkJztcbmltcG9ydCB7IEdMVEZOb2RlLCBWUk1TY2hlbWEgfSBmcm9tICcuLi90eXBlcyc7XG5pbXBvcnQgeyBWUk1DdXJ2ZU1hcHBlciB9IGZyb20gJy4vVlJNQ3VydmVNYXBwZXInO1xuaW1wb3J0IHsgVlJNTG9va0F0QXBwbHllciB9IGZyb20gJy4vVlJNTG9va0F0QXBwbHllcic7XG5pbXBvcnQgeyBWUk1Mb29rQXRIZWFkIH0gZnJvbSAnLi9WUk1Mb29rQXRIZWFkJztcblxuY29uc3QgX2V1bGVyID0gbmV3IFRIUkVFLkV1bGVyKDAuMCwgMC4wLCAwLjAsIFZSTUxvb2tBdEhlYWQuRVVMRVJfT1JERVIpO1xuXG4vKipcbiAqIFRoaXMgY2xhc3MgaXMgdXNlZCBieSBbW1ZSTUxvb2tBdEhlYWRdXSwgYXBwbGllcyBsb29rIGF0IGRpcmVjdGlvbiB0byBleWUgYm9uZXMgb2YgYSBWUk0uXG4gKi9cbmV4cG9ydCBjbGFzcyBWUk1Mb29rQXRCb25lQXBwbHllciBleHRlbmRzIFZSTUxvb2tBdEFwcGx5ZXIge1xuICBwdWJsaWMgcmVhZG9ubHkgdHlwZSA9IFZSTVNjaGVtYS5GaXJzdFBlcnNvbkxvb2tBdFR5cGVOYW1lLkJvbmU7XG5cbiAgcHJpdmF0ZSByZWFkb25seSBfY3VydmVIb3Jpem9udGFsSW5uZXI6IFZSTUN1cnZlTWFwcGVyO1xuICBwcml2YXRlIHJlYWRvbmx5IF9jdXJ2ZUhvcml6b250YWxPdXRlcjogVlJNQ3VydmVNYXBwZXI7XG4gIHByaXZhdGUgcmVhZG9ubHkgX2N1cnZlVmVydGljYWxEb3duOiBWUk1DdXJ2ZU1hcHBlcjtcbiAgcHJpdmF0ZSByZWFkb25seSBfY3VydmVWZXJ0aWNhbFVwOiBWUk1DdXJ2ZU1hcHBlcjtcblxuICBwcml2YXRlIHJlYWRvbmx5IF9sZWZ0RXllOiBHTFRGTm9kZSB8IG51bGw7XG4gIHByaXZhdGUgcmVhZG9ubHkgX3JpZ2h0RXllOiBHTFRGTm9kZSB8IG51bGw7XG5cbiAgLyoqXG4gICAqIENyZWF0ZSBhIG5ldyBWUk1Mb29rQXRCb25lQXBwbHllci5cbiAgICpcbiAgICogQHBhcmFtIGh1bWFub2lkIEEgW1tWUk1IdW1hbm9pZF1dIHVzZWQgYnkgdGhpcyBhcHBsaWVyXG4gICAqIEBwYXJhbSBjdXJ2ZUhvcml6b250YWxJbm5lciBBIFtbVlJNQ3VydmVNYXBwZXJdXSB1c2VkIGZvciBpbm5lciB0cmFuc3ZlcnNlIGRpcmVjdGlvblxuICAgKiBAcGFyYW0gY3VydmVIb3Jpem9udGFsT3V0ZXIgQSBbW1ZSTUN1cnZlTWFwcGVyXV0gdXNlZCBmb3Igb3V0ZXIgdHJhbnN2ZXJzZSBkaXJlY3Rpb25cbiAgICogQHBhcmFtIGN1cnZlVmVydGljYWxEb3duIEEgW1tWUk1DdXJ2ZU1hcHBlcl1dIHVzZWQgZm9yIGRvd24gZGlyZWN0aW9uXG4gICAqIEBwYXJhbSBjdXJ2ZVZlcnRpY2FsVXAgQSBbW1ZSTUN1cnZlTWFwcGVyXV0gdXNlZCBmb3IgdXAgZGlyZWN0aW9uXG4gICAqL1xuICBjb25zdHJ1Y3RvcihcbiAgICBodW1hbm9pZDogVlJNSHVtYW5vaWQsXG4gICAgY3VydmVIb3Jpem9udGFsSW5uZXI6IFZSTUN1cnZlTWFwcGVyLFxuICAgIGN1cnZlSG9yaXpvbnRhbE91dGVyOiBWUk1DdXJ2ZU1hcHBlcixcbiAgICBjdXJ2ZVZlcnRpY2FsRG93bjogVlJNQ3VydmVNYXBwZXIsXG4gICAgY3VydmVWZXJ0aWNhbFVwOiBWUk1DdXJ2ZU1hcHBlcixcbiAgKSB7XG4gICAgc3VwZXIoKTtcblxuICAgIHRoaXMuX2N1cnZlSG9yaXpvbnRhbElubmVyID0gY3VydmVIb3Jpem9udGFsSW5uZXI7XG4gICAgdGhpcy5fY3VydmVIb3Jpem9udGFsT3V0ZXIgPSBjdXJ2ZUhvcml6b250YWxPdXRlcjtcbiAgICB0aGlzLl9jdXJ2ZVZlcnRpY2FsRG93biA9IGN1cnZlVmVydGljYWxEb3duO1xuICAgIHRoaXMuX2N1cnZlVmVydGljYWxVcCA9IGN1cnZlVmVydGljYWxVcDtcblxuICAgIHRoaXMuX2xlZnRFeWUgPSBodW1hbm9pZC5nZXRCb25lTm9kZShWUk1TY2hlbWEuSHVtYW5vaWRCb25lTmFtZS5MZWZ0RXllKTtcbiAgICB0aGlzLl9yaWdodEV5ZSA9IGh1bWFub2lkLmdldEJvbmVOb2RlKFZSTVNjaGVtYS5IdW1hbm9pZEJvbmVOYW1lLlJpZ2h0RXllKTtcbiAgfVxuXG4gIHB1YmxpYyBsb29rQXQoZXVsZXI6IFRIUkVFLkV1bGVyKTogdm9pZCB7XG4gICAgY29uc3Qgc3JjWCA9IGV1bGVyLng7XG4gICAgY29uc3Qgc3JjWSA9IGV1bGVyLnk7XG5cbiAgICAvLyBsZWZ0XG4gICAgaWYgKHRoaXMuX2xlZnRFeWUpIHtcbiAgICAgIGlmIChzcmNYIDwgMC4wKSB7XG4gICAgICAgIF9ldWxlci54ID0gLXRoaXMuX2N1cnZlVmVydGljYWxEb3duLm1hcCgtc3JjWCk7XG4gICAgICB9IGVsc2Uge1xuICAgICAgICBfZXVsZXIueCA9IHRoaXMuX2N1cnZlVmVydGljYWxVcC5tYXAoc3JjWCk7XG4gICAgICB9XG5cbiAgICAgIGlmIChzcmNZIDwgMC4wKSB7XG4gICAgICAgIF9ldWxlci55ID0gLXRoaXMuX2N1cnZlSG9yaXpvbnRhbElubmVyLm1hcCgtc3JjWSk7XG4gICAgICB9IGVsc2Uge1xuICAgICAgICBfZXVsZXIueSA9IHRoaXMuX2N1cnZlSG9yaXpvbnRhbE91dGVyLm1hcChzcmNZKTtcbiAgICAgIH1cblxuICAgICAgdGhpcy5fbGVmdEV5ZS5xdWF0ZXJuaW9uLnNldEZyb21FdWxlcihfZXVsZXIpO1xuICAgIH1cblxuICAgIC8vIHJpZ2h0XG4gICAgaWYgKHRoaXMuX3JpZ2h0RXllKSB7XG4gICAgICBpZiAoc3JjWCA8IDAuMCkge1xuICAgICAgICBfZXVsZXIueCA9IC10aGlzLl9jdXJ2ZVZlcnRpY2FsRG93bi5tYXAoLXNyY1gpO1xuICAgICAgfSBlbHNlIHtcbiAgICAgICAgX2V1bGVyLnggPSB0aGlzLl9jdXJ2ZVZlcnRpY2FsVXAubWFwKHNyY1gpO1xuICAgICAgfVxuXG4gICAgICBpZiAoc3JjWSA8IDAuMCkge1xuICAgICAgICBfZXVsZXIueSA9IC10aGlzLl9jdXJ2ZUhvcml6b250YWxPdXRlci5tYXAoLXNyY1kpO1xuICAgICAgfSBlbHNlIHtcbiAgICAgICAgX2V1bGVyLnkgPSB0aGlzLl9jdXJ2ZUhvcml6b250YWxJbm5lci5tYXAoc3JjWSk7XG4gICAgICB9XG5cbiAgICAgIHRoaXMuX3JpZ2h0RXllLnF1YXRlcm5pb24uc2V0RnJvbUV1bGVyKF9ldWxlcik7XG4gICAgfVxuICB9XG59XG4iLCJpbXBvcnQgeyBHTFRGIH0gZnJvbSAndGhyZWUvZXhhbXBsZXMvanNtL2xvYWRlcnMvR0xURkxvYWRlcic7XG5pbXBvcnQgeyBWUk1CbGVuZFNoYXBlUHJveHkgfSBmcm9tICcuLi9ibGVuZHNoYXBlJztcbmltcG9ydCB7IFZSTUZpcnN0UGVyc29uIH0gZnJvbSAnLi4vZmlyc3RwZXJzb24nO1xuaW1wb3J0IHsgVlJNSHVtYW5vaWQgfSBmcm9tICcuLi9odW1hbm9pZCc7XG5pbXBvcnQgeyBWUk1TY2hlbWEgfSBmcm9tICcuLi90eXBlcyc7XG5pbXBvcnQgeyBWUk1DdXJ2ZU1hcHBlciB9IGZyb20gJy4vVlJNQ3VydmVNYXBwZXInO1xuaW1wb3J0IHsgVlJNTG9va0F0QXBwbHllciB9IGZyb20gJy4vVlJNTG9va0F0QXBwbHllcic7XG5pbXBvcnQgeyBWUk1Mb29rQXRCbGVuZFNoYXBlQXBwbHllciB9IGZyb20gJy4vVlJNTG9va0F0QmxlbmRTaGFwZUFwcGx5ZXInO1xuaW1wb3J0IHsgVlJNTG9va0F0Qm9uZUFwcGx5ZXIgfSBmcm9tICcuL1ZSTUxvb2tBdEJvbmVBcHBseWVyJztcbmltcG9ydCB7IFZSTUxvb2tBdEhlYWQgfSBmcm9tICcuL1ZSTUxvb2tBdEhlYWQnO1xuXG4vLyBUSFJFRS5NYXRoIGhhcyBiZWVuIHJlbmFtZWQgdG8gVEhSRUUuTWF0aFV0aWxzIHNpbmNlIHIxMTMuXG4vLyBXZSBhcmUgZ29pbmcgdG8gZGVmaW5lIHRoZSBERUcyUkFEIGJ5IG91cnNlbHZlcyBmb3IgYSB3aGlsZVxuLy8gaHR0cHM6Ly9naXRodWIuY29tL21yZG9vYi90aHJlZS5qcy9wdWxsLzE4MjcwXG5jb25zdCBERUcyUkFEID0gTWF0aC5QSSAvIDE4MDsgLy8gVEhSRUUuTWF0aFV0aWxzLkRFRzJSQUQ7XG5cbi8qKlxuICogQW4gaW1wb3J0ZXIgdGhhdCBpbXBvcnRzIGEgW1tWUk1Mb29rQXRIZWFkXV0gZnJvbSBhIFZSTSBleHRlbnNpb24gb2YgYSBHTFRGLlxuICovXG5leHBvcnQgY2xhc3MgVlJNTG9va0F0SW1wb3J0ZXIge1xuICAvKipcbiAgICogSW1wb3J0IGEgW1tWUk1Mb29rQXRIZWFkXV0gZnJvbSBhIFZSTS5cbiAgICpcbiAgICogQHBhcmFtIGdsdGYgQSBwYXJzZWQgcmVzdWx0IG9mIEdMVEYgdGFrZW4gZnJvbSBHTFRGTG9hZGVyXG4gICAqIEBwYXJhbSBibGVuZFNoYXBlUHJveHkgQSBbW1ZSTUJsZW5kU2hhcGVQcm94eV1dIGluc3RhbmNlIHRoYXQgcmVwcmVzZW50cyB0aGUgVlJNXG4gICAqIEBwYXJhbSBodW1hbm9pZCBBIFtbVlJNSHVtYW5vaWRdXSBpbnN0YW5jZSB0aGF0IHJlcHJlc2VudHMgdGhlIFZSTVxuICAgKi9cbiAgcHVibGljIGltcG9ydChcbiAgICBnbHRmOiBHTFRGLFxuICAgIGZpcnN0UGVyc29uOiBWUk1GaXJzdFBlcnNvbixcbiAgICBibGVuZFNoYXBlUHJveHk6IFZSTUJsZW5kU2hhcGVQcm94eSxcbiAgICBodW1hbm9pZDogVlJNSHVtYW5vaWQsXG4gICk6IFZSTUxvb2tBdEhlYWQgfCBudWxsIHtcbiAgICBjb25zdCB2cm1FeHQ6IFZSTVNjaGVtYS5WUk0gfCB1bmRlZmluZWQgPSBnbHRmLnBhcnNlci5qc29uLmV4dGVuc2lvbnM/LlZSTTtcbiAgICBpZiAoIXZybUV4dCkge1xuICAgICAgcmV0dXJuIG51bGw7XG4gICAgfVxuXG4gICAgY29uc3Qgc2NoZW1hRmlyc3RQZXJzb246IFZSTVNjaGVtYS5GaXJzdFBlcnNvbiB8IHVuZGVmaW5lZCA9IHZybUV4dC5maXJzdFBlcnNvbjtcbiAgICBpZiAoIXNjaGVtYUZpcnN0UGVyc29uKSB7XG4gICAgICByZXR1cm4gbnVsbDtcbiAgICB9XG5cbiAgICBjb25zdCBhcHBseWVyID0gdGhpcy5faW1wb3J0QXBwbHllcihzY2hlbWFGaXJzdFBlcnNvbiwgYmxlbmRTaGFwZVByb3h5LCBodW1hbm9pZCk7XG4gICAgcmV0dXJuIG5ldyBWUk1Mb29rQXRIZWFkKGZpcnN0UGVyc29uLCBhcHBseWVyIHx8IHVuZGVmaW5lZCk7XG4gIH1cblxuICBwcm90ZWN0ZWQgX2ltcG9ydEFwcGx5ZXIoXG4gICAgc2NoZW1hRmlyc3RQZXJzb246IFZSTVNjaGVtYS5GaXJzdFBlcnNvbixcbiAgICBibGVuZFNoYXBlUHJveHk6IFZSTUJsZW5kU2hhcGVQcm94eSxcbiAgICBodW1hbm9pZDogVlJNSHVtYW5vaWQsXG4gICk6IFZSTUxvb2tBdEFwcGx5ZXIgfCBudWxsIHtcbiAgICBjb25zdCBsb29rQXRIb3Jpem9udGFsSW5uZXIgPSBzY2hlbWFGaXJzdFBlcnNvbi5sb29rQXRIb3Jpem9udGFsSW5uZXI7XG4gICAgY29uc3QgbG9va0F0SG9yaXpvbnRhbE91dGVyID0gc2NoZW1hRmlyc3RQZXJzb24ubG9va0F0SG9yaXpvbnRhbE91dGVyO1xuICAgIGNvbnN0IGxvb2tBdFZlcnRpY2FsRG93biA9IHNjaGVtYUZpcnN0UGVyc29uLmxvb2tBdFZlcnRpY2FsRG93bjtcbiAgICBjb25zdCBsb29rQXRWZXJ0aWNhbFVwID0gc2NoZW1hRmlyc3RQZXJzb24ubG9va0F0VmVydGljYWxVcDtcblxuICAgIHN3aXRjaCAoc2NoZW1hRmlyc3RQZXJzb24ubG9va0F0VHlwZU5hbWUpIHtcbiAgICAgIGNhc2UgVlJNU2NoZW1hLkZpcnN0UGVyc29uTG9va0F0VHlwZU5hbWUuQm9uZToge1xuICAgICAgICBpZiAoXG4gICAgICAgICAgbG9va0F0SG9yaXpvbnRhbElubmVyID09PSB1bmRlZmluZWQgfHxcbiAgICAgICAgICBsb29rQXRIb3Jpem9udGFsT3V0ZXIgPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIGxvb2tBdFZlcnRpY2FsRG93biA9PT0gdW5kZWZpbmVkIHx8XG4gICAgICAgICAgbG9va0F0VmVydGljYWxVcCA9PT0gdW5kZWZpbmVkXG4gICAgICAgICkge1xuICAgICAgICAgIHJldHVybiBudWxsO1xuICAgICAgICB9IGVsc2Uge1xuICAgICAgICAgIHJldHVybiBuZXcgVlJNTG9va0F0Qm9uZUFwcGx5ZXIoXG4gICAgICAgICAgICBodW1hbm9pZCxcbiAgICAgICAgICAgIHRoaXMuX2ltcG9ydEN1cnZlTWFwcGVyQm9uZShsb29rQXRIb3Jpem9udGFsSW5uZXIpLFxuICAgICAgICAgICAgdGhpcy5faW1wb3J0Q3VydmVNYXBwZXJCb25lKGxvb2tBdEhvcml6b250YWxPdXRlciksXG4gICAgICAgICAgICB0aGlzLl9pbXBvcnRDdXJ2ZU1hcHBlckJvbmUobG9va0F0VmVydGljYWxEb3duKSxcbiAgICAgICAgICAgIHRoaXMuX2ltcG9ydEN1cnZlTWFwcGVyQm9uZShsb29rQXRWZXJ0aWNhbFVwKSxcbiAgICAgICAgICApO1xuICAgICAgICB9XG4gICAgICB9XG4gICAgICBjYXNlIFZSTVNjaGVtYS5GaXJzdFBlcnNvbkxvb2tBdFR5cGVOYW1lLkJsZW5kU2hhcGU6IHtcbiAgICAgICAgaWYgKGxvb2tBdEhvcml6b250YWxPdXRlciA9PT0gdW5kZWZpbmVkIHx8IGxvb2tBdFZlcnRpY2FsRG93biA9PT0gdW5kZWZpbmVkIHx8IGxvb2tBdFZlcnRpY2FsVXAgPT09IHVuZGVmaW5lZCkge1xuICAgICAgICAgIHJldHVybiBudWxsO1xuICAgICAgICB9IGVsc2Uge1xuICAgICAgICAgIHJldHVybiBuZXcgVlJNTG9va0F0QmxlbmRTaGFwZUFwcGx5ZXIoXG4gICAgICAgICAgICBibGVuZFNoYXBlUHJveHksXG4gICAgICAgICAgICB0aGlzLl9pbXBvcnRDdXJ2ZU1hcHBlckJsZW5kU2hhcGUobG9va0F0SG9yaXpvbnRhbE91dGVyKSxcbiAgICAgICAgICAgIHRoaXMuX2ltcG9ydEN1cnZlTWFwcGVyQmxlbmRTaGFwZShsb29rQXRWZXJ0aWNhbERvd24pLFxuICAgICAgICAgICAgdGhpcy5faW1wb3J0Q3VydmVNYXBwZXJCbGVuZFNoYXBlKGxvb2tBdFZlcnRpY2FsVXApLFxuICAgICAgICAgICk7XG4gICAgICAgIH1cbiAgICAgIH1cbiAgICAgIGRlZmF1bHQ6IHtcbiAgICAgICAgcmV0dXJuIG51bGw7XG4gICAgICB9XG4gICAgfVxuICB9XG5cbiAgcHJpdmF0ZSBfaW1wb3J0Q3VydmVNYXBwZXJCb25lKG1hcDogVlJNU2NoZW1hLkZpcnN0UGVyc29uRGVncmVlTWFwKTogVlJNQ3VydmVNYXBwZXIge1xuICAgIHJldHVybiBuZXcgVlJNQ3VydmVNYXBwZXIoXG4gICAgICB0eXBlb2YgbWFwLnhSYW5nZSA9PT0gJ251bWJlcicgPyBERUcyUkFEICogbWFwLnhSYW5nZSA6IHVuZGVmaW5lZCxcbiAgICAgIHR5cGVvZiBtYXAueVJhbmdlID09PSAnbnVtYmVyJyA/IERFRzJSQUQgKiBtYXAueVJhbmdlIDogdW5kZWZpbmVkLFxuICAgICAgbWFwLmN1cnZlLFxuICAgICk7XG4gIH1cblxuICBwcml2YXRlIF9pbXBvcnRDdXJ2ZU1hcHBlckJsZW5kU2hhcGUobWFwOiBWUk1TY2hlbWEuRmlyc3RQZXJzb25EZWdyZWVNYXApOiBWUk1DdXJ2ZU1hcHBlciB7XG4gICAgcmV0dXJuIG5ldyBWUk1DdXJ2ZU1hcHBlcih0eXBlb2YgbWFwLnhSYW5nZSA9PT0gJ251bWJlcicgPyBERUcyUkFEICogbWFwLnhSYW5nZSA6IHVuZGVmaW5lZCwgbWFwLnlSYW5nZSwgbWFwLmN1cnZlKTtcbiAgfVxufVxuIiwiaW1wb3J0ICogYXMgVEhSRUUgZnJvbSAndGhyZWUnO1xuXG5leHBvcnQgY29uc3QgZ2V0RW5jb2RpbmdDb21wb25lbnRzID0gKGVuY29kaW5nOiBUSFJFRS5UZXh0dXJlRW5jb2RpbmcpOiBbc3RyaW5nLCBzdHJpbmddID0+IHtcbiAgc3dpdGNoIChlbmNvZGluZykge1xuICAgIGNhc2UgVEhSRUUuTGluZWFyRW5jb2Rpbmc6XG4gICAgICByZXR1cm4gWydMaW5lYXInLCAnKCB2YWx1ZSApJ107XG4gICAgY2FzZSBUSFJFRS5zUkdCRW5jb2Rpbmc6XG4gICAgICByZXR1cm4gWydzUkdCJywgJyggdmFsdWUgKSddO1xuICAgIGNhc2UgVEhSRUUuUkdCRUVuY29kaW5nOlxuICAgICAgcmV0dXJuIFsnUkdCRScsICcoIHZhbHVlICknXTtcbiAgICBjYXNlIFRIUkVFLlJHQk03RW5jb2Rpbmc6XG4gICAgICByZXR1cm4gWydSR0JNJywgJyggdmFsdWUsIDcuMCApJ107XG4gICAgY2FzZSBUSFJFRS5SR0JNMTZFbmNvZGluZzpcbiAgICAgIHJldHVybiBbJ1JHQk0nLCAnKCB2YWx1ZSwgMTYuMCApJ107XG4gICAgY2FzZSBUSFJFRS5SR0JERW5jb2Rpbmc6XG4gICAgICByZXR1cm4gWydSR0JEJywgJyggdmFsdWUsIDI1Ni4wICknXTtcbiAgICBjYXNlIFRIUkVFLkdhbW1hRW5jb2Rpbmc6XG4gICAgICByZXR1cm4gWydHYW1tYScsICcoIHZhbHVlLCBmbG9hdCggR0FNTUFfRkFDVE9SICkgKSddO1xuICAgIGRlZmF1bHQ6XG4gICAgICB0aHJvdyBuZXcgRXJyb3IoJ3Vuc3VwcG9ydGVkIGVuY29kaW5nOiAnICsgZW5jb2RpbmcpO1xuICB9XG59O1xuXG5leHBvcnQgY29uc3QgZ2V0VGV4ZWxEZWNvZGluZ0Z1bmN0aW9uID0gKGZ1bmN0aW9uTmFtZTogc3RyaW5nLCBlbmNvZGluZzogVEhSRUUuVGV4dHVyZUVuY29kaW5nKTogc3RyaW5nID0+IHtcbiAgY29uc3QgY29tcG9uZW50cyA9IGdldEVuY29kaW5nQ29tcG9uZW50cyhlbmNvZGluZyk7XG4gIHJldHVybiAndmVjNCAnICsgZnVuY3Rpb25OYW1lICsgJyggdmVjNCB2YWx1ZSApIHsgcmV0dXJuICcgKyBjb21wb25lbnRzWzBdICsgJ1RvTGluZWFyJyArIGNvbXBvbmVudHNbMV0gKyAnOyB9Jztcbn07XG4iLCIvKiB0c2xpbnQ6ZGlzYWJsZTptZW1iZXItb3JkZXJpbmcgKi9cblxuaW1wb3J0ICogYXMgVEhSRUUgZnJvbSAndGhyZWUnO1xuaW1wb3J0IHsgZ2V0VGV4ZWxEZWNvZGluZ0Z1bmN0aW9uIH0gZnJvbSAnLi9nZXRUZXhlbERlY29kaW5nRnVuY3Rpb24nO1xuaW1wb3J0IHZlcnRleFNoYWRlciBmcm9tICcuL3NoYWRlcnMvbXRvb24udmVydCc7XG5pbXBvcnQgZnJhZ21lbnRTaGFkZXIgZnJvbSAnLi9zaGFkZXJzL210b29uLmZyYWcnO1xuXG5jb25zdCBUQVUgPSAyLjAgKiBNYXRoLlBJO1xuXG5leHBvcnQgaW50ZXJmYWNlIE1Ub29uUGFyYW1ldGVycyBleHRlbmRzIFRIUkVFLlNoYWRlck1hdGVyaWFsUGFyYW1ldGVycyB7XG4gIG1Ub29uVmVyc2lvbj86IG51bWJlcjsgLy8gX01Ub29uVmVyc2lvblxuXG4gIGN1dG9mZj86IG51bWJlcjsgLy8gX0N1dG9mZlxuICBjb2xvcj86IFRIUkVFLlZlY3RvcjQ7IC8vIHJnYiBvZiBfQ29sb3JcbiAgc2hhZGVDb2xvcj86IFRIUkVFLlZlY3RvcjQ7IC8vIF9TaGFkZUNvbG9yXG4gIG1hcD86IFRIUkVFLlRleHR1cmU7IC8vIF9NYWluVGV4XG4gIG1haW5UZXg/OiBUSFJFRS5UZXh0dXJlOyAvLyBfTWFpblRleCAod2lsbCBiZSByZW5hbWVkIHRvIG1hcClcbiAgLy8gZXNsaW50LWRpc2FibGUtbmV4dC1saW5lIEB0eXBlc2NyaXB0LWVzbGludC9uYW1pbmctY29udmVudGlvblxuICBtYWluVGV4X1NUPzogVEhSRUUuVmVjdG9yNDsgLy8gX01haW5UZXhfU1RcbiAgc2hhZGVUZXh0dXJlPzogVEhSRUUuVGV4dHVyZTsgLy8gX1NoYWRlVGV4dHVyZVxuICBidW1wU2NhbGU/OiBudW1iZXI7IC8vIF9CdW1wU2NhbGUgKHdpbGwgYmUgY29udmVydGVkIHRvIG5vcm1hbFNjYWxlKVxuICBub3JtYWxNYXA/OiBUSFJFRS5UZXh0dXJlOyAvLyBfQnVtcE1hcFxuICBub3JtYWxNYXBUeXBlPzogVEhSRUUuTm9ybWFsTWFwVHlwZXM7IC8vIFRocmVlLmpzIHNwZWNpZmljIHZhbHVlXG4gIG5vcm1hbFNjYWxlPzogVEhSRUUuVmVjdG9yMjsgLy8gX0J1bXBTY2FsZSBpbiBUaHJlZS5qcyBmYXNoaW9uXG4gIGJ1bXBNYXA/OiBUSFJFRS5UZXh0dXJlOyAvLyBfQnVtcE1hcCAod2lsbCBiZSByZW5hbWVkIHRvIG5vcm1hbE1hcClcbiAgcmVjZWl2ZVNoYWRvd1JhdGU/OiBudW1iZXI7IC8vIF9SZWNlaXZlU2hhZG93UmF0ZVxuICByZWNlaXZlU2hhZG93VGV4dHVyZT86IFRIUkVFLlRleHR1cmU7IC8vIF9SZWNlaXZlU2hhZG93VGV4dHVyZVxuICBzaGFkaW5nR3JhZGVSYXRlPzogbnVtYmVyOyAvLyBfU2hhZGluZ0dyYWRlUmF0ZVxuICBzaGFkaW5nR3JhZGVUZXh0dXJlPzogVEhSRUUuVGV4dHVyZTsgLy8gX1NoYWRpbmdHcmFkZVRleHR1cmVcbiAgc2hhZGVTaGlmdD86IG51bWJlcjsgLy8gX1NoYWRlU2hpZnRcbiAgc2hhZGVUb29ueT86IG51bWJlcjsgLy8gX1NoYWRlVG9vbnlcbiAgbGlnaHRDb2xvckF0dGVudWF0aW9uPzogbnVtYmVyOyAvLyBfTGlnaHRDb2xvckF0dGVudWF0aW9uXG4gIGluZGlyZWN0TGlnaHRJbnRlbnNpdHk/OiBudW1iZXI7IC8vIF9JbmRpcmVjdExpZ2h0SW50ZW5zaXR5XG4gIHJpbVRleHR1cmU/OiBUSFJFRS5UZXh0dXJlOyAvLyBfUmltVGV4dHVyZVxuICByaW1Db2xvcj86IFRIUkVFLlZlY3RvcjQ7IC8vIF9SaW1Db2xvclxuICByaW1MaWdodGluZ01peD86IG51bWJlcjsgLy8gX1JpbUxpZ2h0aW5nTWl4XG4gIHJpbUZyZXNuZWxQb3dlcj86IG51bWJlcjsgLy8gX1JpbUZyZXNuZWxQb3dlclxuICByaW1MaWZ0PzogbnVtYmVyOyAvLyBfUmltTGlmdFxuICBzcGhlcmVBZGQ/OiBUSFJFRS5UZXh0dXJlOyAvLyBfU3BoZXJlQWRkXG4gIGVtaXNzaW9uQ29sb3I/OiBUSFJFRS5WZWN0b3I0OyAvLyBfRW1pc3Npb25Db2xvclxuICBlbWlzc2l2ZU1hcD86IFRIUkVFLlRleHR1cmU7IC8vIF9FbWlzc2lvbk1hcFxuICBlbWlzc2lvbk1hcD86IFRIUkVFLlRleHR1cmU7IC8vIF9FbWlzc2lvbk1hcCAod2lsbCBiZSByZW5hbWVkIHRvIGVtaXNzaXZlTWFwKVxuICBvdXRsaW5lV2lkdGhUZXh0dXJlPzogVEhSRUUuVGV4dHVyZTsgLy8gX091dGxpbmVXaWR0aFRleHR1cmVcbiAgb3V0bGluZVdpZHRoPzogbnVtYmVyOyAvLyBfT3V0bGluZVdpZHRoXG4gIG91dGxpbmVTY2FsZWRNYXhEaXN0YW5jZT86IG51bWJlcjsgLy8gX091dGxpbmVTY2FsZWRNYXhEaXN0YW5jZVxuICBvdXRsaW5lQ29sb3I/OiBUSFJFRS5WZWN0b3I0OyAvLyBfT3V0bGluZUNvbG9yXG4gIG91dGxpbmVMaWdodGluZ01peD86IG51bWJlcjsgLy8gX091dGxpbmVMaWdodGluZ01peFxuICB1dkFuaW1NYXNrVGV4dHVyZT86IFRIUkVFLlRleHR1cmU7IC8vIF9VdkFuaW1NYXNrVGV4dHVyZVxuICB1dkFuaW1TY3JvbGxYPzogbnVtYmVyOyAvLyBfVXZBbmltU2Nyb2xsWFxuICB1dkFuaW1TY3JvbGxZPzogbnVtYmVyOyAvLyBfVXZBbmltU2Nyb2xsWVxuICB1dkFuaW1Sb3RhdGlvbj86IG51bWJlcjsgLy8gX3V2QW5pbVJvdGF0aW9uXG5cbiAgZGVidWdNb2RlPzogTVRvb25NYXRlcmlhbERlYnVnTW9kZSB8IG51bWJlcjsgLy8gX0RlYnVnTW9kZVxuICBibGVuZE1vZGU/OiBNVG9vbk1hdGVyaWFsUmVuZGVyTW9kZSB8IG51bWJlcjsgLy8gX0JsZW5kTW9kZVxuICBvdXRsaW5lV2lkdGhNb2RlPzogTVRvb25NYXRlcmlhbE91dGxpbmVXaWR0aE1vZGUgfCBudW1iZXI7IC8vIE91dGxpbmVXaWR0aE1vZGVcbiAgb3V0bGluZUNvbG9yTW9kZT86IE1Ub29uTWF0ZXJpYWxPdXRsaW5lQ29sb3JNb2RlIHwgbnVtYmVyOyAvLyBPdXRsaW5lQ29sb3JNb2RlXG4gIGN1bGxNb2RlPzogTVRvb25NYXRlcmlhbEN1bGxNb2RlIHwgbnVtYmVyOyAvLyBfQ3VsbE1vZGVcbiAgb3V0bGluZUN1bGxNb2RlPzogTVRvb25NYXRlcmlhbEN1bGxNb2RlIHwgbnVtYmVyOyAvLyBfT3V0bGluZUN1bGxNb2RlXG4gIHNyY0JsZW5kPzogbnVtYmVyOyAvLyBfU3JjQmxlbmRcbiAgZHN0QmxlbmQ/OiBudW1iZXI7IC8vIF9Ec3RCbGVuZFxuICB6V3JpdGU/OiBudW1iZXI7IC8vIF9aV3JpdGUgKHdpbGwgYmUgcmVuYW1lZCB0byBkZXB0aFdyaXRlKVxuXG4gIGlzT3V0bGluZT86IGJvb2xlYW47XG5cbiAgLyoqXG4gICAqIFNwZWNpZnkgdGhlIGVuY29kaW5nIG9mIGlucHV0IHVuaWZvcm0gY29sb3JzLlxuICAgKlxuICAgKiBXaGVuIHlvdXIgYHJlbmRlcmVyLm91dHB1dEVuY29kaW5nYCBpcyBgVEhSRUUuTGluZWFyRW5jb2RpbmdgLCB1c2UgYFRIUkVFLkxpbmVhckVuY29kaW5nYC5cbiAgICogV2hlbiB5b3VyIGByZW5kZXJlci5vdXRwdXRFbmNvZGluZ2AgaXMgYFRIUkVFLnNSR0JFbmNvZGluZ2AsIHVzZSBgVEhSRUUuc1JHQkVuY29kaW5nYC5cbiAgICpcbiAgICogRW5jb2RpbmdzIG9mIHRleHR1cmVzIHNob3VsZCBiZSBzZXQgaW5kZXBlbmRlbnRseSBvbiB0ZXh0dXJlcy5cbiAgICpcbiAgICogVGhpcyB3aWxsIHVzZSBgVEhSRUUuTGluZWFyRW5jb2RpbmdgIGlmIHRoaXMgb3B0aW9uIGlzbid0IHNwZWNpZmllZC5cbiAgICpcbiAgICogU2VlIGFsc286IGh0dHBzOi8vdGhyZWVqcy5vcmcvZG9jcy8jYXBpL2VuL3JlbmRlcmVycy9XZWJHTFJlbmRlcmVyLm91dHB1dEVuY29kaW5nXG4gICAqL1xuICBlbmNvZGluZz86IFRIUkVFLlRleHR1cmVFbmNvZGluZztcbn1cblxuZXhwb3J0IGVudW0gTVRvb25NYXRlcmlhbEN1bGxNb2RlIHtcbiAgT2ZmLFxuICBGcm9udCxcbiAgQmFjayxcbn1cblxuZXhwb3J0IGVudW0gTVRvb25NYXRlcmlhbERlYnVnTW9kZSB7XG4gIE5vbmUsXG4gIE5vcm1hbCxcbiAgTGl0U2hhZGVSYXRlLFxuICBVVixcbn1cblxuZXhwb3J0IGVudW0gTVRvb25NYXRlcmlhbE91dGxpbmVDb2xvck1vZGUge1xuICBGaXhlZENvbG9yLFxuICBNaXhlZExpZ2h0aW5nLFxufVxuXG5leHBvcnQgZW51bSBNVG9vbk1hdGVyaWFsT3V0bGluZVdpZHRoTW9kZSB7XG4gIE5vbmUsXG4gIFdvcmxkQ29vcmRpbmF0ZXMsXG4gIFNjcmVlbkNvb3JkaW5hdGVzLFxufVxuXG5leHBvcnQgZW51bSBNVG9vbk1hdGVyaWFsUmVuZGVyTW9kZSB7XG4gIE9wYXF1ZSxcbiAgQ3V0b3V0LFxuICBUcmFuc3BhcmVudCxcbiAgVHJhbnNwYXJlbnRXaXRoWldyaXRlLFxufVxuXG4vKipcbiAqIE1Ub29uIGlzIGEgbWF0ZXJpYWwgc3BlY2lmaWNhdGlvbiB0aGF0IGhhcyB2YXJpb3VzIGZlYXR1cmVzLlxuICogVGhlIHNwZWMgYW5kIGltcGxlbWVudGF0aW9uIGFyZSBvcmlnaW5hbGx5IGZvdW5kZWQgZm9yIFVuaXR5IGVuZ2luZSBhbmQgdGhpcyBpcyBhIHBvcnQgb2YgdGhlIG1hdGVyaWFsLlxuICpcbiAqIFNlZTogaHR0cHM6Ly9naXRodWIuY29tL1NhbnRhcmgvTVRvb25cbiAqL1xuZXhwb3J0IGNsYXNzIE1Ub29uTWF0ZXJpYWwgZXh0ZW5kcyBUSFJFRS5TaGFkZXJNYXRlcmlhbCB7XG4gIC8qKlxuICAgKiBSZWFkb25seSBib29sZWFuIHRoYXQgaW5kaWNhdGVzIHRoaXMgaXMgYSBbW01Ub29uTWF0ZXJpYWxdXS5cbiAgICovXG4gIHB1YmxpYyByZWFkb25seSBpc01Ub29uTWF0ZXJpYWw6IGJvb2xlYW4gPSB0cnVlO1xuXG4gIHB1YmxpYyBjdXRvZmYgPSAwLjU7IC8vIF9DdXRvZmZcbiAgcHVibGljIGNvbG9yID0gbmV3IFRIUkVFLlZlY3RvcjQoMS4wLCAxLjAsIDEuMCwgMS4wKTsgLy8gX0NvbG9yXG4gIHB1YmxpYyBzaGFkZUNvbG9yID0gbmV3IFRIUkVFLlZlY3RvcjQoMC45NywgMC44MSwgMC44NiwgMS4wKTsgLy8gX1NoYWRlQ29sb3JcbiAgcHVibGljIG1hcDogVEhSRUUuVGV4dHVyZSB8IG51bGwgPSBudWxsOyAvLyBfTWFpblRleFxuICAvLyBlc2xpbnQtZGlzYWJsZS1uZXh0LWxpbmUgQHR5cGVzY3JpcHQtZXNsaW50L25hbWluZy1jb252ZW50aW9uXG4gIHB1YmxpYyBtYWluVGV4X1NUID0gbmV3IFRIUkVFLlZlY3RvcjQoMC4wLCAwLjAsIDEuMCwgMS4wKTsgLy8gX01haW5UZXhfU1RcbiAgcHVibGljIHNoYWRlVGV4dHVyZTogVEhSRUUuVGV4dHVyZSB8IG51bGwgPSBudWxsOyAvLyBfU2hhZGVUZXh0dXJlXG4gIC8vIHB1YmxpYyBzaGFkZVRleHR1cmVfU1QgPSBuZXcgVEhSRUUuVmVjdG9yNCgwLjAsIDAuMCwgMS4wLCAxLjApOyAvLyBfU2hhZGVUZXh0dXJlX1NUICh1bnVzZWQpXG4gIHB1YmxpYyBub3JtYWxNYXA6IFRIUkVFLlRleHR1cmUgfCBudWxsID0gbnVsbDsgLy8gX0J1bXBNYXAuIGFnYWluLCBUSElTIElTIF9CdW1wTWFwXG4gIHB1YmxpYyBub3JtYWxNYXBUeXBlID0gVEhSRUUuVGFuZ2VudFNwYWNlTm9ybWFsTWFwOyAvLyBUaHJlZS5qcyByZXF1aXJlcyB0aGlzXG4gIHB1YmxpYyBub3JtYWxTY2FsZSA9IG5ldyBUSFJFRS5WZWN0b3IyKDEuMCwgMS4wKTsgLy8gX0J1bXBTY2FsZSwgaW4gVmVjdG9yMlxuICAvLyBwdWJsaWMgYnVtcE1hcF9TVCA9IG5ldyBUSFJFRS5WZWN0b3I0KDAuMCwgMC4wLCAxLjAsIDEuMCk7IC8vIF9CdW1wTWFwX1NUICh1bnVzZWQpXG4gIHB1YmxpYyByZWNlaXZlU2hhZG93UmF0ZSA9IDEuMDsgLy8gX1JlY2VpdmVTaGFkb3dSYXRlXG4gIHB1YmxpYyByZWNlaXZlU2hhZG93VGV4dHVyZTogVEhSRUUuVGV4dHVyZSB8IG51bGwgPSBudWxsOyAvLyBfUmVjZWl2ZVNoYWRvd1RleHR1cmVcbiAgLy8gcHVibGljIHJlY2VpdmVTaGFkb3dUZXh0dXJlX1NUID0gbmV3IFRIUkVFLlZlY3RvcjQoMC4wLCAwLjAsIDEuMCwgMS4wKTsgLy8gX1JlY2VpdmVTaGFkb3dUZXh0dXJlX1NUICh1bnVzZWQpXG4gIHB1YmxpYyBzaGFkaW5nR3JhZGVSYXRlID0gMS4wOyAvLyBfU2hhZGluZ0dyYWRlUmF0ZVxuICBwdWJsaWMgc2hhZGluZ0dyYWRlVGV4dHVyZTogVEhSRUUuVGV4dHVyZSB8IG51bGwgPSBudWxsOyAvLyBfU2hhZGluZ0dyYWRlVGV4dHVyZVxuICAvLyBwdWJsaWMgc2hhZGluZ0dyYWRlVGV4dHVyZV9TVCA9IG5ldyBUSFJFRS5WZWN0b3I0KDAuMCwgMC4wLCAxLjAsIDEuMCk7IC8vIF9TaGFkaW5nR3JhZGVUZXh0dXJlX1NUICh1bnVzZWQpXG4gIHB1YmxpYyBzaGFkZVNoaWZ0ID0gMC4wOyAvLyBfU2hhZGVTaGlmdFxuICBwdWJsaWMgc2hhZGVUb29ueSA9IDAuOTsgLy8gX1NoYWRlVG9vbnlcbiAgcHVibGljIGxpZ2h0Q29sb3JBdHRlbnVhdGlvbiA9IDAuMDsgLy8gX0xpZ2h0Q29sb3JBdHRlbnVhdGlvblxuICBwdWJsaWMgaW5kaXJlY3RMaWdodEludGVuc2l0eSA9IDAuMTsgLy8gX0luZGlyZWN0TGlnaHRJbnRlbnNpdHlcbiAgcHVibGljIHJpbVRleHR1cmU6IFRIUkVFLlRleHR1cmUgfCBudWxsID0gbnVsbDsgLy8gX1JpbVRleHR1cmVcbiAgcHVibGljIHJpbUNvbG9yID0gbmV3IFRIUkVFLlZlY3RvcjQoMC4wLCAwLjAsIDAuMCwgMS4wKTsgLy8gX1JpbUNvbG9yXG4gIHB1YmxpYyByaW1MaWdodGluZ01peCA9IDAuMDsgLy8gX1JpbUxpZ2h0aW5nTWl4XG4gIHB1YmxpYyByaW1GcmVzbmVsUG93ZXIgPSAxLjA7IC8vIF9SaW1GcmVzbmVsUG93ZXJcbiAgcHVibGljIHJpbUxpZnQgPSAwLjA7IC8vIF9SaW1MaWZ0XG4gIHB1YmxpYyBzcGhlcmVBZGQ6IFRIUkVFLlRleHR1cmUgfCBudWxsID0gbnVsbDsgLy8gX1NwaGVyZUFkZFxuICAvLyBwdWJsaWMgc3BoZXJlQWRkX1NUID0gbmV3IFRIUkVFLlZlY3RvcjQoMC4wLCAwLjAsIDEuMCwgMS4wKTsgLy8gX1NwaGVyZUFkZF9TVCAodW51c2VkKVxuICBwdWJsaWMgZW1pc3Npb25Db2xvciA9IG5ldyBUSFJFRS5WZWN0b3I0KDAuMCwgMC4wLCAwLjAsIDEuMCk7IC8vIF9FbWlzc2lvbkNvbG9yXG4gIHB1YmxpYyBlbWlzc2l2ZU1hcDogVEhSRUUuVGV4dHVyZSB8IG51bGwgPSBudWxsOyAvLyBfRW1pc3Npb25NYXBcbiAgLy8gcHVibGljIGVtaXNzaW9uTWFwX1NUID0gbmV3IFRIUkVFLlZlY3RvcjQoMC4wLCAwLjAsIDEuMCwgMS4wKTsgLy8gX0VtaXNzaW9uTWFwX1NUICh1bnVzZWQpXG4gIHB1YmxpYyBvdXRsaW5lV2lkdGhUZXh0dXJlOiBUSFJFRS5UZXh0dXJlIHwgbnVsbCA9IG51bGw7IC8vIF9PdXRsaW5lV2lkdGhUZXh0dXJlXG4gIC8vIHB1YmxpYyBvdXRsaW5lV2lkdGhUZXh0dXJlX1NUID0gbmV3IFRIUkVFLlZlY3RvcjQoMC4wLCAwLjAsIDEuMCwgMS4wKTsgLy8gX091dGxpbmVXaWR0aFRleHR1cmVfU1QgKHVudXNlZClcbiAgcHVibGljIG91dGxpbmVXaWR0aCA9IDAuNTsgLy8gX091dGxpbmVXaWR0aFxuICBwdWJsaWMgb3V0bGluZVNjYWxlZE1heERpc3RhbmNlID0gMS4wOyAvLyBfT3V0bGluZVNjYWxlZE1heERpc3RhbmNlXG4gIHB1YmxpYyBvdXRsaW5lQ29sb3IgPSBuZXcgVEhSRUUuVmVjdG9yNCgwLjAsIDAuMCwgMC4wLCAxLjApOyAvLyBfT3V0bGluZUNvbG9yXG4gIHB1YmxpYyBvdXRsaW5lTGlnaHRpbmdNaXggPSAxLjA7IC8vIF9PdXRsaW5lTGlnaHRpbmdNaXhcbiAgcHVibGljIHV2QW5pbU1hc2tUZXh0dXJlOiBUSFJFRS5UZXh0dXJlIHwgbnVsbCA9IG51bGw7IC8vIF9VdkFuaW1NYXNrVGV4dHVyZVxuICBwdWJsaWMgdXZBbmltU2Nyb2xsWCA9IDAuMDsgLy8gX1V2QW5pbVNjcm9sbFhcbiAgcHVibGljIHV2QW5pbVNjcm9sbFkgPSAwLjA7IC8vIF9VdkFuaW1TY3JvbGxZXG4gIHB1YmxpYyB1dkFuaW1Sb3RhdGlvbiA9IDAuMDsgLy8gX3V2QW5pbVJvdGF0aW9uXG5cbiAgcHVibGljIHNob3VsZEFwcGx5VW5pZm9ybXMgPSB0cnVlOyAvLyB3aGVuIHRoaXMgaXMgdHJ1ZSwgYXBwbHlVbmlmb3JtcyBlZmZlY3RzXG5cbiAgLyoqXG4gICAqIFRoZSBlbmNvZGluZyBvZiBpbnB1dCB1bmlmb3JtIGNvbG9ycy5cbiAgICpcbiAgICogV2hlbiB5b3VyIGByZW5kZXJlci5vdXRwdXRFbmNvZGluZ2AgaXMgYFRIUkVFLkxpbmVhckVuY29kaW5nYCwgdXNlIGBUSFJFRS5MaW5lYXJFbmNvZGluZ2AuXG4gICAqIFdoZW4geW91ciBgcmVuZGVyZXIub3V0cHV0RW5jb2RpbmdgIGlzIGBUSFJFRS5zUkdCRW5jb2RpbmdgLCB1c2UgYFRIUkVFLnNSR0JFbmNvZGluZ2AuXG4gICAqXG4gICAqIEVuY29kaW5ncyBvZiB0ZXh0dXJlcyBhcmUgc2V0IGluZGVwZW5kZW50bHkgb24gdGV4dHVyZXMuXG4gICAqXG4gICAqIFRoaXMgaXMgYFRIUkVFLkxpbmVhckVuY29kaW5nYCBieSBkZWZhdWx0LlxuICAgKlxuICAgKiBTZWUgYWxzbzogaHR0cHM6Ly90aHJlZWpzLm9yZy9kb2NzLyNhcGkvZW4vcmVuZGVyZXJzL1dlYkdMUmVuZGVyZXIub3V0cHV0RW5jb2RpbmdcbiAgICovXG4gIHB1YmxpYyBlbmNvZGluZzogVEhSRUUuVGV4dHVyZUVuY29kaW5nO1xuXG4gIHByaXZhdGUgX2RlYnVnTW9kZSA9IE1Ub29uTWF0ZXJpYWxEZWJ1Z01vZGUuTm9uZTsgLy8gX0RlYnVnTW9kZVxuICBwcml2YXRlIF9ibGVuZE1vZGUgPSBNVG9vbk1hdGVyaWFsUmVuZGVyTW9kZS5PcGFxdWU7IC8vIF9CbGVuZE1vZGVcbiAgcHJpdmF0ZSBfb3V0bGluZVdpZHRoTW9kZSA9IE1Ub29uTWF0ZXJpYWxPdXRsaW5lV2lkdGhNb2RlLk5vbmU7IC8vIF9PdXRsaW5lV2lkdGhNb2RlXG4gIHByaXZhdGUgX291dGxpbmVDb2xvck1vZGUgPSBNVG9vbk1hdGVyaWFsT3V0bGluZUNvbG9yTW9kZS5GaXhlZENvbG9yOyAvLyBfT3V0bGluZUNvbG9yTW9kZVxuICBwcml2YXRlIF9jdWxsTW9kZSA9IE1Ub29uTWF0ZXJpYWxDdWxsTW9kZS5CYWNrOyAvLyBfQ3VsbE1vZGVcbiAgcHJpdmF0ZSBfb3V0bGluZUN1bGxNb2RlID0gTVRvb25NYXRlcmlhbEN1bGxNb2RlLkZyb250OyAvLyBfT3V0bGluZUN1bGxNb2RlXG4gIC8vIHB1YmxpYyBzcmNCbGVuZCA9IDEuMDsgLy8gX1NyY0JsZW5kIChpcyBub3Qgc3VwcG9ydGVkKVxuICAvLyBwdWJsaWMgZHN0QmxlbmQgPSAwLjA7IC8vIF9Ec3RCbGVuZCAoaXMgbm90IHN1cHBvcnRlZClcbiAgLy8gcHVibGljIHpXcml0ZSA9IDEuMDsgLy8gX1pXcml0ZSAod2lsbCBiZSBjb252ZXJ0ZWQgdG8gZGVwdGhXcml0ZSlcblxuICBwcml2YXRlIF9pc091dGxpbmUgPSBmYWxzZTtcblxuICBwcml2YXRlIF91dkFuaW1PZmZzZXRYID0gMC4wO1xuICBwcml2YXRlIF91dkFuaW1PZmZzZXRZID0gMC4wO1xuICBwcml2YXRlIF91dkFuaW1QaGFzZSA9IDAuMDtcblxuICBjb25zdHJ1Y3RvcihwYXJhbWV0ZXJzOiBNVG9vblBhcmFtZXRlcnMgPSB7fSkge1xuICAgIHN1cGVyKCk7XG5cbiAgICB0aGlzLmVuY29kaW5nID0gcGFyYW1ldGVycy5lbmNvZGluZyB8fCBUSFJFRS5MaW5lYXJFbmNvZGluZztcbiAgICBpZiAodGhpcy5lbmNvZGluZyAhPT0gVEhSRUUuTGluZWFyRW5jb2RpbmcgJiYgdGhpcy5lbmNvZGluZyAhPT0gVEhSRUUuc1JHQkVuY29kaW5nKSB7XG4gICAgICBjb25zb2xlLndhcm4oXG4gICAgICAgICdUaGUgc3BlY2lmaWVkIGNvbG9yIGVuY29kaW5nIGRvZXMgbm90IHdvcmsgcHJvcGVybHkgd2l0aCBNVG9vbk1hdGVyaWFsLiBZb3UgbWlnaHQgd2FudCB0byB1c2UgVEhSRUUuc1JHQkVuY29kaW5nIGluc3RlYWQuJyxcbiAgICAgICk7XG4gICAgfVxuXG4gICAgLy8gPT0gdGhlc2UgcGFyYW1ldGVyIGhhcyBubyBjb21wYXRpYmlsaXR5IHdpdGggdGhpcyBpbXBsZW1lbnRhdGlvbiA9PT09PT09PVxuICAgIFtcbiAgICAgICdtVG9vblZlcnNpb24nLFxuICAgICAgJ3NoYWRlVGV4dHVyZV9TVCcsXG4gICAgICAnYnVtcE1hcF9TVCcsXG4gICAgICAncmVjZWl2ZVNoYWRvd1RleHR1cmVfU1QnLFxuICAgICAgJ3NoYWRpbmdHcmFkZVRleHR1cmVfU1QnLFxuICAgICAgJ3JpbVRleHR1cmVfU1QnLFxuICAgICAgJ3NwaGVyZUFkZF9TVCcsXG4gICAgICAnZW1pc3Npb25NYXBfU1QnLFxuICAgICAgJ291dGxpbmVXaWR0aFRleHR1cmVfU1QnLFxuICAgICAgJ3V2QW5pbU1hc2tUZXh0dXJlX1NUJyxcbiAgICAgICdzcmNCbGVuZCcsXG4gICAgICAnZHN0QmxlbmQnLFxuICAgIF0uZm9yRWFjaCgoa2V5KSA9PiB7XG4gICAgICBpZiAoKHBhcmFtZXRlcnMgYXMgYW55KVtrZXldICE9PSB1bmRlZmluZWQpIHtcbiAgICAgICAgLy8gY29uc29sZS53YXJuKGBUSFJFRS4ke3RoaXMudHlwZX06IFRoZSBwYXJhbWV0ZXIgXCIke2tleX1cIiBpcyBub3Qgc3VwcG9ydGVkLmApO1xuICAgICAgICBkZWxldGUgKHBhcmFtZXRlcnMgYXMgYW55KVtrZXldO1xuICAgICAgfVxuICAgIH0pO1xuXG4gICAgLy8gPT0gZW5hYmxpbmcgYnVuY2ggb2Ygc3R1ZmYgPT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PVxuICAgIHBhcmFtZXRlcnMuZm9nID0gdHJ1ZTtcbiAgICBwYXJhbWV0ZXJzLmxpZ2h0cyA9IHRydWU7XG4gICAgcGFyYW1ldGVycy5jbGlwcGluZyA9IHRydWU7XG5cbiAgICBwYXJhbWV0ZXJzLnNraW5uaW5nID0gcGFyYW1ldGVycy5za2lubmluZyB8fCBmYWxzZTtcbiAgICBwYXJhbWV0ZXJzLm1vcnBoVGFyZ2V0cyA9IHBhcmFtZXRlcnMubW9ycGhUYXJnZXRzIHx8IGZhbHNlO1xuICAgIHBhcmFtZXRlcnMubW9ycGhOb3JtYWxzID0gcGFyYW1ldGVycy5tb3JwaE5vcm1hbHMgfHwgZmFsc2U7XG5cbiAgICAvLyA9PSB1bmlmb3JtcyA9PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09XG4gICAgcGFyYW1ldGVycy51bmlmb3JtcyA9IFRIUkVFLlVuaWZvcm1zVXRpbHMubWVyZ2UoW1xuICAgICAgVEhSRUUuVW5pZm9ybXNMaWIuY29tbW9uLCAvLyBtYXBcbiAgICAgIFRIUkVFLlVuaWZvcm1zTGliLm5vcm1hbG1hcCwgLy8gbm9ybWFsTWFwXG4gICAgICBUSFJFRS5Vbmlmb3Jtc0xpYi5lbWlzc2l2ZW1hcCwgLy8gZW1pc3NpdmVNYXBcbiAgICAgIFRIUkVFLlVuaWZvcm1zTGliLmZvZyxcbiAgICAgIFRIUkVFLlVuaWZvcm1zTGliLmxpZ2h0cyxcbiAgICAgIHtcbiAgICAgICAgY3V0b2ZmOiB7IHZhbHVlOiAwLjUgfSxcbiAgICAgICAgY29sb3I6IHsgdmFsdWU6IG5ldyBUSFJFRS5Db2xvcigxLjAsIDEuMCwgMS4wKSB9LFxuICAgICAgICBjb2xvckFscGhhOiB7IHZhbHVlOiAxLjAgfSxcbiAgICAgICAgc2hhZGVDb2xvcjogeyB2YWx1ZTogbmV3IFRIUkVFLkNvbG9yKDAuOTcsIDAuODEsIDAuODYpIH0sXG4gICAgICAgIC8vIGVzbGludC1kaXNhYmxlLW5leHQtbGluZSBAdHlwZXNjcmlwdC1lc2xpbnQvbmFtaW5nLWNvbnZlbnRpb25cbiAgICAgICAgbWFpblRleF9TVDogeyB2YWx1ZTogbmV3IFRIUkVFLlZlY3RvcjQoMC4wLCAwLjAsIDEuMCwgMS4wKSB9LFxuICAgICAgICBzaGFkZVRleHR1cmU6IHsgdmFsdWU6IG51bGwgfSxcbiAgICAgICAgcmVjZWl2ZVNoYWRvd1JhdGU6IHsgdmFsdWU6IDEuMCB9LFxuICAgICAgICByZWNlaXZlU2hhZG93VGV4dHVyZTogeyB2YWx1ZTogbnVsbCB9LFxuICAgICAgICBzaGFkaW5nR3JhZGVSYXRlOiB7IHZhbHVlOiAxLjAgfSxcbiAgICAgICAgc2hhZGluZ0dyYWRlVGV4dHVyZTogeyB2YWx1ZTogbnVsbCB9LFxuICAgICAgICBzaGFkZVNoaWZ0OiB7IHZhbHVlOiAwLjAgfSxcbiAgICAgICAgc2hhZGVUb29ueTogeyB2YWx1ZTogMC45IH0sXG4gICAgICAgIGxpZ2h0Q29sb3JBdHRlbnVhdGlvbjogeyB2YWx1ZTogMC4wIH0sXG4gICAgICAgIGluZGlyZWN0TGlnaHRJbnRlbnNpdHk6IHsgdmFsdWU6IDAuMSB9LFxuICAgICAgICByaW1UZXh0dXJlOiB7IHZhbHVlOiBudWxsIH0sXG4gICAgICAgIHJpbUNvbG9yOiB7IHZhbHVlOiBuZXcgVEhSRUUuQ29sb3IoMC4wLCAwLjAsIDAuMCkgfSxcbiAgICAgICAgcmltTGlnaHRpbmdNaXg6IHsgdmFsdWU6IDAuMCB9LFxuICAgICAgICByaW1GcmVzbmVsUG93ZXI6IHsgdmFsdWU6IDEuMCB9LFxuICAgICAgICByaW1MaWZ0OiB7IHZhbHVlOiAwLjAgfSxcbiAgICAgICAgc3BoZXJlQWRkOiB7IHZhbHVlOiBudWxsIH0sXG4gICAgICAgIGVtaXNzaW9uQ29sb3I6IHsgdmFsdWU6IG5ldyBUSFJFRS5Db2xvcigwLjAsIDAuMCwgMC4wKSB9LFxuICAgICAgICBvdXRsaW5lV2lkdGhUZXh0dXJlOiB7IHZhbHVlOiBudWxsIH0sXG4gICAgICAgIG91dGxpbmVXaWR0aDogeyB2YWx1ZTogMC41IH0sXG4gICAgICAgIG91dGxpbmVTY2FsZWRNYXhEaXN0YW5jZTogeyB2YWx1ZTogMS4wIH0sXG4gICAgICAgIG91dGxpbmVDb2xvcjogeyB2YWx1ZTogbmV3IFRIUkVFLkNvbG9yKDAuMCwgMC4wLCAwLjApIH0sXG4gICAgICAgIG91dGxpbmVMaWdodGluZ01peDogeyB2YWx1ZTogMS4wIH0sXG4gICAgICAgIHV2QW5pbU1hc2tUZXh0dXJlOiB7IHZhbHVlOiBudWxsIH0sXG4gICAgICAgIHV2QW5pbU9mZnNldFg6IHsgdmFsdWU6IDAuMCB9LFxuICAgICAgICB1dkFuaW1PZmZzZXRZOiB7IHZhbHVlOiAwLjAgfSxcbiAgICAgICAgdXZBbmltVGhldGE6IHsgdmFsdWU6IDAuMCB9LFxuICAgICAgfSxcbiAgICBdKTtcblxuICAgIC8vID09IGZpbmFsbHkgY29tcGlsZSB0aGUgc2hhZGVyIHByb2dyYW0gPT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT1cbiAgICB0aGlzLnNldFZhbHVlcyhwYXJhbWV0ZXJzKTtcblxuICAgIC8vID09IHVwZGF0ZSBzaGFkZXIgc3R1ZmYgPT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT1cbiAgICB0aGlzLl91cGRhdGVTaGFkZXJDb2RlKCk7XG4gICAgdGhpcy5fYXBwbHlVbmlmb3JtcygpO1xuICB9XG5cbiAgZ2V0IG1haW5UZXgoKTogVEhSRUUuVGV4dHVyZSB8IG51bGwge1xuICAgIHJldHVybiB0aGlzLm1hcDtcbiAgfVxuXG4gIHNldCBtYWluVGV4KHQ6IFRIUkVFLlRleHR1cmUgfCBudWxsKSB7XG4gICAgdGhpcy5tYXAgPSB0O1xuICB9XG5cbiAgZ2V0IGJ1bXBNYXAoKTogVEhSRUUuVGV4dHVyZSB8IG51bGwge1xuICAgIHJldHVybiB0aGlzLm5vcm1hbE1hcDtcbiAgfVxuXG4gIHNldCBidW1wTWFwKHQ6IFRIUkVFLlRleHR1cmUgfCBudWxsKSB7XG4gICAgdGhpcy5ub3JtYWxNYXAgPSB0O1xuICB9XG5cbiAgLyoqXG4gICAqIEdldHRpbmcgdGhlIGBidW1wU2NhbGVgIHJldXRybnMgaXRzIHggY29tcG9uZW50IG9mIGBub3JtYWxTY2FsZWAgKGFzc3VtaW5nIHggYW5kIHkgY29tcG9uZW50IG9mIGBub3JtYWxTY2FsZWAgYXJlIHNhbWUpLlxuICAgKi9cbiAgZ2V0IGJ1bXBTY2FsZSgpOiBudW1iZXIge1xuICAgIHJldHVybiB0aGlzLm5vcm1hbFNjYWxlLng7XG4gIH1cblxuICAvKipcbiAgICogU2V0dGluZyB0aGUgYGJ1bXBTY2FsZWAgd2lsbCBiZSBjb252ZXJ0IHRoZSB2YWx1ZSBpbnRvIFZlY3RvcjIgYG5vcm1hbFNjYWxlYCAuXG4gICAqL1xuICBzZXQgYnVtcFNjYWxlKHQ6IG51bWJlcikge1xuICAgIHRoaXMubm9ybWFsU2NhbGUuc2V0KHQsIHQpO1xuICB9XG5cbiAgZ2V0IGVtaXNzaW9uTWFwKCk6IFRIUkVFLlRleHR1cmUgfCBudWxsIHtcbiAgICByZXR1cm4gdGhpcy5lbWlzc2l2ZU1hcDtcbiAgfVxuXG4gIHNldCBlbWlzc2lvbk1hcCh0OiBUSFJFRS5UZXh0dXJlIHwgbnVsbCkge1xuICAgIHRoaXMuZW1pc3NpdmVNYXAgPSB0O1xuICB9XG5cbiAgZ2V0IGJsZW5kTW9kZSgpOiBNVG9vbk1hdGVyaWFsUmVuZGVyTW9kZSB7XG4gICAgcmV0dXJuIHRoaXMuX2JsZW5kTW9kZTtcbiAgfVxuXG4gIHNldCBibGVuZE1vZGUobTogTVRvb25NYXRlcmlhbFJlbmRlck1vZGUpIHtcbiAgICB0aGlzLl9ibGVuZE1vZGUgPSBtO1xuXG4gICAgdGhpcy5kZXB0aFdyaXRlID0gdGhpcy5fYmxlbmRNb2RlICE9PSBNVG9vbk1hdGVyaWFsUmVuZGVyTW9kZS5UcmFuc3BhcmVudDtcbiAgICB0aGlzLnRyYW5zcGFyZW50ID1cbiAgICAgIHRoaXMuX2JsZW5kTW9kZSA9PT0gTVRvb25NYXRlcmlhbFJlbmRlck1vZGUuVHJhbnNwYXJlbnQgfHxcbiAgICAgIHRoaXMuX2JsZW5kTW9kZSA9PT0gTVRvb25NYXRlcmlhbFJlbmRlck1vZGUuVHJhbnNwYXJlbnRXaXRoWldyaXRlO1xuICAgIHRoaXMuX3VwZGF0ZVNoYWRlckNvZGUoKTtcbiAgfVxuXG4gIGdldCBkZWJ1Z01vZGUoKTogTVRvb25NYXRlcmlhbERlYnVnTW9kZSB7XG4gICAgcmV0dXJuIHRoaXMuX2RlYnVnTW9kZTtcbiAgfVxuXG4gIHNldCBkZWJ1Z01vZGUobTogTVRvb25NYXRlcmlhbERlYnVnTW9kZSkge1xuICAgIHRoaXMuX2RlYnVnTW9kZSA9IG07XG5cbiAgICB0aGlzLl91cGRhdGVTaGFkZXJDb2RlKCk7XG4gIH1cblxuICBnZXQgb3V0bGluZVdpZHRoTW9kZSgpOiBNVG9vbk1hdGVyaWFsT3V0bGluZVdpZHRoTW9kZSB7XG4gICAgcmV0dXJuIHRoaXMuX291dGxpbmVXaWR0aE1vZGU7XG4gIH1cblxuICBzZXQgb3V0bGluZVdpZHRoTW9kZShtOiBNVG9vbk1hdGVyaWFsT3V0bGluZVdpZHRoTW9kZSkge1xuICAgIHRoaXMuX291dGxpbmVXaWR0aE1vZGUgPSBtO1xuXG4gICAgdGhpcy5fdXBkYXRlU2hhZGVyQ29kZSgpO1xuICB9XG5cbiAgZ2V0IG91dGxpbmVDb2xvck1vZGUoKTogTVRvb25NYXRlcmlhbE91dGxpbmVDb2xvck1vZGUge1xuICAgIHJldHVybiB0aGlzLl9vdXRsaW5lQ29sb3JNb2RlO1xuICB9XG5cbiAgc2V0IG91dGxpbmVDb2xvck1vZGUobTogTVRvb25NYXRlcmlhbE91dGxpbmVDb2xvck1vZGUpIHtcbiAgICB0aGlzLl9vdXRsaW5lQ29sb3JNb2RlID0gbTtcblxuICAgIHRoaXMuX3VwZGF0ZVNoYWRlckNvZGUoKTtcbiAgfVxuXG4gIGdldCBjdWxsTW9kZSgpOiBNVG9vbk1hdGVyaWFsQ3VsbE1vZGUge1xuICAgIHJldHVybiB0aGlzLl9jdWxsTW9kZTtcbiAgfVxuXG4gIHNldCBjdWxsTW9kZShtOiBNVG9vbk1hdGVyaWFsQ3VsbE1vZGUpIHtcbiAgICB0aGlzLl9jdWxsTW9kZSA9IG07XG5cbiAgICB0aGlzLl91cGRhdGVDdWxsRmFjZSgpO1xuICB9XG5cbiAgZ2V0IG91dGxpbmVDdWxsTW9kZSgpOiBNVG9vbk1hdGVyaWFsQ3VsbE1vZGUge1xuICAgIHJldHVybiB0aGlzLl9vdXRsaW5lQ3VsbE1vZGU7XG4gIH1cblxuICBzZXQgb3V0bGluZUN1bGxNb2RlKG06IE1Ub29uTWF0ZXJpYWxDdWxsTW9kZSkge1xuICAgIHRoaXMuX291dGxpbmVDdWxsTW9kZSA9IG07XG5cbiAgICB0aGlzLl91cGRhdGVDdWxsRmFjZSgpO1xuICB9XG5cbiAgZ2V0IHpXcml0ZSgpOiBudW1iZXIge1xuICAgIHJldHVybiB0aGlzLmRlcHRoV3JpdGUgPyAxIDogMDtcbiAgfVxuXG4gIHNldCB6V3JpdGUoaTogbnVtYmVyKSB7XG4gICAgdGhpcy5kZXB0aFdyaXRlID0gMC41IDw9IGk7XG4gIH1cblxuICBnZXQgaXNPdXRsaW5lKCk6IGJvb2xlYW4ge1xuICAgIHJldHVybiB0aGlzLl9pc091dGxpbmU7XG4gIH1cblxuICBzZXQgaXNPdXRsaW5lKGI6IGJvb2xlYW4pIHtcbiAgICB0aGlzLl9pc091dGxpbmUgPSBiO1xuXG4gICAgdGhpcy5fdXBkYXRlU2hhZGVyQ29kZSgpO1xuICAgIHRoaXMuX3VwZGF0ZUN1bGxGYWNlKCk7XG4gIH1cblxuICAvKipcbiAgICogVXBkYXRlIHRoaXMgbWF0ZXJpYWwuXG4gICAqIFVzdWFsbHkgdGhpcyB3aWxsIGJlIGNhbGxlZCB2aWEgW1tWUk0udXBkYXRlXV0gc28geW91IGRvbid0IGhhdmUgdG8gY2FsbCB0aGlzIG1hbnVhbGx5LlxuICAgKlxuICAgKiBAcGFyYW0gZGVsdGEgZGVsdGFUaW1lIHNpbmNlIGxhc3QgdXBkYXRlXG4gICAqL1xuICBwdWJsaWMgdXBkYXRlVlJNTWF0ZXJpYWxzKGRlbHRhOiBudW1iZXIpOiB2b2lkIHtcbiAgICB0aGlzLl91dkFuaW1PZmZzZXRYID0gdGhpcy5fdXZBbmltT2Zmc2V0WCArIGRlbHRhICogdGhpcy51dkFuaW1TY3JvbGxYO1xuICAgIHRoaXMuX3V2QW5pbU9mZnNldFkgPSB0aGlzLl91dkFuaW1PZmZzZXRZIC0gZGVsdGEgKiB0aGlzLnV2QW5pbVNjcm9sbFk7IC8vIE5lZ2F0aXZlIHNpbmNlIHQgYXhpcyBvZiB1dnMgYXJlIG9wcG9zaXRlIGZyb20gVW5pdHkncyBvbmVcbiAgICB0aGlzLl91dkFuaW1QaGFzZSA9IHRoaXMuX3V2QW5pbVBoYXNlICsgZGVsdGEgKiB0aGlzLnV2QW5pbVJvdGF0aW9uO1xuXG4gICAgdGhpcy5fYXBwbHlVbmlmb3JtcygpO1xuICB9XG5cbiAgcHVibGljIGNvcHkoc291cmNlOiB0aGlzKTogdGhpcyB7XG4gICAgc3VwZXIuY29weShzb3VyY2UpO1xuXG4gICAgLy8gPT0gY29weSBtZW1iZXJzID09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PVxuICAgIHRoaXMuY3V0b2ZmID0gc291cmNlLmN1dG9mZjtcbiAgICB0aGlzLmNvbG9yLmNvcHkoc291cmNlLmNvbG9yKTtcbiAgICB0aGlzLnNoYWRlQ29sb3IuY29weShzb3VyY2Uuc2hhZGVDb2xvcik7XG4gICAgdGhpcy5tYXAgPSBzb3VyY2UubWFwO1xuICAgIHRoaXMubWFpblRleF9TVC5jb3B5KHNvdXJjZS5tYWluVGV4X1NUKTtcbiAgICB0aGlzLnNoYWRlVGV4dHVyZSA9IHNvdXJjZS5zaGFkZVRleHR1cmU7XG4gICAgdGhpcy5ub3JtYWxNYXAgPSBzb3VyY2Uubm9ybWFsTWFwO1xuICAgIHRoaXMubm9ybWFsTWFwVHlwZSA9IHNvdXJjZS5ub3JtYWxNYXBUeXBlO1xuICAgIHRoaXMubm9ybWFsU2NhbGUuY29weSh0aGlzLm5vcm1hbFNjYWxlKTtcbiAgICB0aGlzLnJlY2VpdmVTaGFkb3dSYXRlID0gc291cmNlLnJlY2VpdmVTaGFkb3dSYXRlO1xuICAgIHRoaXMucmVjZWl2ZVNoYWRvd1RleHR1cmUgPSBzb3VyY2UucmVjZWl2ZVNoYWRvd1RleHR1cmU7XG4gICAgdGhpcy5zaGFkaW5nR3JhZGVSYXRlID0gc291cmNlLnNoYWRpbmdHcmFkZVJhdGU7XG4gICAgdGhpcy5zaGFkaW5nR3JhZGVUZXh0dXJlID0gc291cmNlLnNoYWRpbmdHcmFkZVRleHR1cmU7XG4gICAgdGhpcy5zaGFkZVNoaWZ0ID0gc291cmNlLnNoYWRlU2hpZnQ7XG4gICAgdGhpcy5zaGFkZVRvb255ID0gc291cmNlLnNoYWRlVG9vbnk7XG4gICAgdGhpcy5saWdodENvbG9yQXR0ZW51YXRpb24gPSBzb3VyY2UubGlnaHRDb2xvckF0dGVudWF0aW9uO1xuICAgIHRoaXMuaW5kaXJlY3RMaWdodEludGVuc2l0eSA9IHNvdXJjZS5pbmRpcmVjdExpZ2h0SW50ZW5zaXR5O1xuICAgIHRoaXMucmltVGV4dHVyZSA9IHNvdXJjZS5yaW1UZXh0dXJlO1xuICAgIHRoaXMucmltQ29sb3IuY29weShzb3VyY2UucmltQ29sb3IpO1xuICAgIHRoaXMucmltTGlnaHRpbmdNaXggPSBzb3VyY2UucmltTGlnaHRpbmdNaXg7XG4gICAgdGhpcy5yaW1GcmVzbmVsUG93ZXIgPSBzb3VyY2UucmltRnJlc25lbFBvd2VyO1xuICAgIHRoaXMucmltTGlmdCA9IHNvdXJjZS5yaW1MaWZ0O1xuICAgIHRoaXMuc3BoZXJlQWRkID0gc291cmNlLnNwaGVyZUFkZDtcbiAgICB0aGlzLmVtaXNzaW9uQ29sb3IuY29weShzb3VyY2UuZW1pc3Npb25Db2xvcik7XG4gICAgdGhpcy5lbWlzc2l2ZU1hcCA9IHNvdXJjZS5lbWlzc2l2ZU1hcDtcbiAgICB0aGlzLm91dGxpbmVXaWR0aFRleHR1cmUgPSBzb3VyY2Uub3V0bGluZVdpZHRoVGV4dHVyZTtcbiAgICB0aGlzLm91dGxpbmVXaWR0aCA9IHNvdXJjZS5vdXRsaW5lV2lkdGg7XG4gICAgdGhpcy5vdXRsaW5lU2NhbGVkTWF4RGlzdGFuY2UgPSBzb3VyY2Uub3V0bGluZVNjYWxlZE1heERpc3RhbmNlO1xuICAgIHRoaXMub3V0bGluZUNvbG9yLmNvcHkoc291cmNlLm91dGxpbmVDb2xvcik7XG4gICAgdGhpcy5vdXRsaW5lTGlnaHRpbmdNaXggPSBzb3VyY2Uub3V0bGluZUxpZ2h0aW5nTWl4O1xuICAgIHRoaXMudXZBbmltTWFza1RleHR1cmUgPSBzb3VyY2UudXZBbmltTWFza1RleHR1cmU7XG4gICAgdGhpcy51dkFuaW1TY3JvbGxYID0gc291cmNlLnV2QW5pbVNjcm9sbFg7XG4gICAgdGhpcy51dkFuaW1TY3JvbGxZID0gc291cmNlLnV2QW5pbVNjcm9sbFk7XG4gICAgdGhpcy51dkFuaW1Sb3RhdGlvbiA9IHNvdXJjZS51dkFuaW1Sb3RhdGlvbjtcblxuICAgIHRoaXMuZGVidWdNb2RlID0gc291cmNlLmRlYnVnTW9kZTtcbiAgICB0aGlzLmJsZW5kTW9kZSA9IHNvdXJjZS5ibGVuZE1vZGU7XG4gICAgdGhpcy5vdXRsaW5lV2lkdGhNb2RlID0gc291cmNlLm91dGxpbmVXaWR0aE1vZGU7XG4gICAgdGhpcy5vdXRsaW5lQ29sb3JNb2RlID0gc291cmNlLm91dGxpbmVDb2xvck1vZGU7XG4gICAgdGhpcy5jdWxsTW9kZSA9IHNvdXJjZS5jdWxsTW9kZTtcbiAgICB0aGlzLm91dGxpbmVDdWxsTW9kZSA9IHNvdXJjZS5vdXRsaW5lQ3VsbE1vZGU7XG5cbiAgICB0aGlzLmlzT3V0bGluZSA9IHNvdXJjZS5pc091dGxpbmU7XG5cbiAgICByZXR1cm4gdGhpcztcbiAgfVxuXG4gIC8qKlxuICAgKiBBcHBseSB1cGRhdGVkIHVuaWZvcm0gdmFyaWFibGVzLlxuICAgKi9cbiAgcHJpdmF0ZSBfYXBwbHlVbmlmb3JtcygpOiB2b2lkIHtcbiAgICB0aGlzLnVuaWZvcm1zLnV2QW5pbU9mZnNldFgudmFsdWUgPSB0aGlzLl91dkFuaW1PZmZzZXRYO1xuICAgIHRoaXMudW5pZm9ybXMudXZBbmltT2Zmc2V0WS52YWx1ZSA9IHRoaXMuX3V2QW5pbU9mZnNldFk7XG4gICAgdGhpcy51bmlmb3Jtcy51dkFuaW1UaGV0YS52YWx1ZSA9IFRBVSAqIHRoaXMuX3V2QW5pbVBoYXNlO1xuXG4gICAgaWYgKCF0aGlzLnNob3VsZEFwcGx5VW5pZm9ybXMpIHtcbiAgICAgIHJldHVybjtcbiAgICB9XG4gICAgdGhpcy5zaG91bGRBcHBseVVuaWZvcm1zID0gZmFsc2U7XG5cbiAgICB0aGlzLnVuaWZvcm1zLmN1dG9mZi52YWx1ZSA9IHRoaXMuY3V0b2ZmO1xuICAgIHRoaXMudW5pZm9ybXMuY29sb3IudmFsdWUuc2V0UkdCKHRoaXMuY29sb3IueCwgdGhpcy5jb2xvci55LCB0aGlzLmNvbG9yLnopO1xuICAgIHRoaXMudW5pZm9ybXMuY29sb3JBbHBoYS52YWx1ZSA9IHRoaXMuY29sb3IudztcbiAgICB0aGlzLnVuaWZvcm1zLnNoYWRlQ29sb3IudmFsdWUuc2V0UkdCKHRoaXMuc2hhZGVDb2xvci54LCB0aGlzLnNoYWRlQ29sb3IueSwgdGhpcy5zaGFkZUNvbG9yLnopO1xuICAgIHRoaXMudW5pZm9ybXMubWFwLnZhbHVlID0gdGhpcy5tYXA7XG4gICAgdGhpcy51bmlmb3Jtcy5tYWluVGV4X1NULnZhbHVlLmNvcHkodGhpcy5tYWluVGV4X1NUKTtcbiAgICB0aGlzLnVuaWZvcm1zLnNoYWRlVGV4dHVyZS52YWx1ZSA9IHRoaXMuc2hhZGVUZXh0dXJlO1xuICAgIHRoaXMudW5pZm9ybXMubm9ybWFsTWFwLnZhbHVlID0gdGhpcy5ub3JtYWxNYXA7XG4gICAgdGhpcy51bmlmb3Jtcy5ub3JtYWxTY2FsZS52YWx1ZS5jb3B5KHRoaXMubm9ybWFsU2NhbGUpO1xuICAgIHRoaXMudW5pZm9ybXMucmVjZWl2ZVNoYWRvd1JhdGUudmFsdWUgPSB0aGlzLnJlY2VpdmVTaGFkb3dSYXRlO1xuICAgIHRoaXMudW5pZm9ybXMucmVjZWl2ZVNoYWRvd1RleHR1cmUudmFsdWUgPSB0aGlzLnJlY2VpdmVTaGFkb3dUZXh0dXJlO1xuICAgIHRoaXMudW5pZm9ybXMuc2hhZGluZ0dyYWRlUmF0ZS52YWx1ZSA9IHRoaXMuc2hhZGluZ0dyYWRlUmF0ZTtcbiAgICB0aGlzLnVuaWZvcm1zLnNoYWRpbmdHcmFkZVRleHR1cmUudmFsdWUgPSB0aGlzLnNoYWRpbmdHcmFkZVRleHR1cmU7XG4gICAgdGhpcy51bmlmb3Jtcy5zaGFkZVNoaWZ0LnZhbHVlID0gdGhpcy5zaGFkZVNoaWZ0O1xuICAgIHRoaXMudW5pZm9ybXMuc2hhZGVUb29ueS52YWx1ZSA9IHRoaXMuc2hhZGVUb29ueTtcbiAgICB0aGlzLnVuaWZvcm1zLmxpZ2h0Q29sb3JBdHRlbnVhdGlvbi52YWx1ZSA9IHRoaXMubGlnaHRDb2xvckF0dGVudWF0aW9uO1xuICAgIHRoaXMudW5pZm9ybXMuaW5kaXJlY3RMaWdodEludGVuc2l0eS52YWx1ZSA9IHRoaXMuaW5kaXJlY3RMaWdodEludGVuc2l0eTtcbiAgICB0aGlzLnVuaWZvcm1zLnJpbVRleHR1cmUudmFsdWUgPSB0aGlzLnJpbVRleHR1cmU7XG4gICAgdGhpcy51bmlmb3Jtcy5yaW1Db2xvci52YWx1ZS5zZXRSR0IodGhpcy5yaW1Db2xvci54LCB0aGlzLnJpbUNvbG9yLnksIHRoaXMucmltQ29sb3Iueik7XG4gICAgdGhpcy51bmlmb3Jtcy5yaW1MaWdodGluZ01peC52YWx1ZSA9IHRoaXMucmltTGlnaHRpbmdNaXg7XG4gICAgdGhpcy51bmlmb3Jtcy5yaW1GcmVzbmVsUG93ZXIudmFsdWUgPSB0aGlzLnJpbUZyZXNuZWxQb3dlcjtcbiAgICB0aGlzLnVuaWZvcm1zLnJpbUxpZnQudmFsdWUgPSB0aGlzLnJpbUxpZnQ7XG4gICAgdGhpcy51bmlmb3Jtcy5zcGhlcmVBZGQudmFsdWUgPSB0aGlzLnNwaGVyZUFkZDtcbiAgICB0aGlzLnVuaWZvcm1zLmVtaXNzaW9uQ29sb3IudmFsdWUuc2V0UkdCKHRoaXMuZW1pc3Npb25Db2xvci54LCB0aGlzLmVtaXNzaW9uQ29sb3IueSwgdGhpcy5lbWlzc2lvbkNvbG9yLnopO1xuICAgIHRoaXMudW5pZm9ybXMuZW1pc3NpdmVNYXAudmFsdWUgPSB0aGlzLmVtaXNzaXZlTWFwO1xuICAgIHRoaXMudW5pZm9ybXMub3V0bGluZVdpZHRoVGV4dHVyZS52YWx1ZSA9IHRoaXMub3V0bGluZVdpZHRoVGV4dHVyZTtcbiAgICB0aGlzLnVuaWZvcm1zLm91dGxpbmVXaWR0aC52YWx1ZSA9IHRoaXMub3V0bGluZVdpZHRoO1xuICAgIHRoaXMudW5pZm9ybXMub3V0bGluZVNjYWxlZE1heERpc3RhbmNlLnZhbHVlID0gdGhpcy5vdXRsaW5lU2NhbGVkTWF4RGlzdGFuY2U7XG4gICAgdGhpcy51bmlmb3Jtcy5vdXRsaW5lQ29sb3IudmFsdWUuc2V0UkdCKHRoaXMub3V0bGluZUNvbG9yLngsIHRoaXMub3V0bGluZUNvbG9yLnksIHRoaXMub3V0bGluZUNvbG9yLnopO1xuICAgIHRoaXMudW5pZm9ybXMub3V0bGluZUxpZ2h0aW5nTWl4LnZhbHVlID0gdGhpcy5vdXRsaW5lTGlnaHRpbmdNaXg7XG4gICAgdGhpcy51bmlmb3Jtcy51dkFuaW1NYXNrVGV4dHVyZS52YWx1ZSA9IHRoaXMudXZBbmltTWFza1RleHR1cmU7XG5cbiAgICAvLyBhcHBseSBjb2xvciBzcGFjZSB0byB1bmlmb3JtIGNvbG9yc1xuICAgIGlmICh0aGlzLmVuY29kaW5nID09PSBUSFJFRS5zUkdCRW5jb2RpbmcpIHtcbiAgICAgIHRoaXMudW5pZm9ybXMuY29sb3IudmFsdWUuY29udmVydFNSR0JUb0xpbmVhcigpO1xuICAgICAgdGhpcy51bmlmb3Jtcy5zaGFkZUNvbG9yLnZhbHVlLmNvbnZlcnRTUkdCVG9MaW5lYXIoKTtcbiAgICAgIHRoaXMudW5pZm9ybXMucmltQ29sb3IudmFsdWUuY29udmVydFNSR0JUb0xpbmVhcigpO1xuICAgICAgdGhpcy51bmlmb3Jtcy5lbWlzc2lvbkNvbG9yLnZhbHVlLmNvbnZlcnRTUkdCVG9MaW5lYXIoKTtcbiAgICAgIHRoaXMudW5pZm9ybXMub3V0bGluZUNvbG9yLnZhbHVlLmNvbnZlcnRTUkdCVG9MaW5lYXIoKTtcbiAgICB9XG5cbiAgICB0aGlzLl91cGRhdGVDdWxsRmFjZSgpO1xuICB9XG5cbiAgcHJpdmF0ZSBfdXBkYXRlU2hhZGVyQ29kZSgpOiB2b2lkIHtcbiAgICB0aGlzLmRlZmluZXMgPSB7XG4gICAgICBPVVRMSU5FOiB0aGlzLl9pc091dGxpbmUsXG4gICAgICBCTEVORE1PREVfT1BBUVVFOiB0aGlzLl9ibGVuZE1vZGUgPT09IE1Ub29uTWF0ZXJpYWxSZW5kZXJNb2RlLk9wYXF1ZSxcbiAgICAgIEJMRU5ETU9ERV9DVVRPVVQ6IHRoaXMuX2JsZW5kTW9kZSA9PT0gTVRvb25NYXRlcmlhbFJlbmRlck1vZGUuQ3V0b3V0LFxuICAgICAgQkxFTkRNT0RFX1RSQU5TUEFSRU5UOlxuICAgICAgICB0aGlzLl9ibGVuZE1vZGUgPT09IE1Ub29uTWF0ZXJpYWxSZW5kZXJNb2RlLlRyYW5zcGFyZW50IHx8XG4gICAgICAgIHRoaXMuX2JsZW5kTW9kZSA9PT0gTVRvb25NYXRlcmlhbFJlbmRlck1vZGUuVHJhbnNwYXJlbnRXaXRoWldyaXRlLFxuICAgICAgVVNFX1NIQURFVEVYVFVSRTogdGhpcy5zaGFkZVRleHR1cmUgIT09IG51bGwsXG4gICAgICBVU0VfUkVDRUlWRVNIQURPV1RFWFRVUkU6IHRoaXMucmVjZWl2ZVNoYWRvd1RleHR1cmUgIT09IG51bGwsXG4gICAgICBVU0VfU0hBRElOR0dSQURFVEVYVFVSRTogdGhpcy5zaGFkaW5nR3JhZGVUZXh0dXJlICE9PSBudWxsLFxuICAgICAgVVNFX1JJTVRFWFRVUkU6IHRoaXMucmltVGV4dHVyZSAhPT0gbnVsbCxcbiAgICAgIFVTRV9TUEhFUkVBREQ6IHRoaXMuc3BoZXJlQWRkICE9PSBudWxsLFxuICAgICAgVVNFX09VVExJTkVXSURUSFRFWFRVUkU6IHRoaXMub3V0bGluZVdpZHRoVGV4dHVyZSAhPT0gbnVsbCxcbiAgICAgIFVTRV9VVkFOSU1NQVNLVEVYVFVSRTogdGhpcy51dkFuaW1NYXNrVGV4dHVyZSAhPT0gbnVsbCxcbiAgICAgIERFQlVHX05PUk1BTDogdGhpcy5fZGVidWdNb2RlID09PSBNVG9vbk1hdGVyaWFsRGVidWdNb2RlLk5vcm1hbCxcbiAgICAgIERFQlVHX0xJVFNIQURFUkFURTogdGhpcy5fZGVidWdNb2RlID09PSBNVG9vbk1hdGVyaWFsRGVidWdNb2RlLkxpdFNoYWRlUmF0ZSxcbiAgICAgIERFQlVHX1VWOiB0aGlzLl9kZWJ1Z01vZGUgPT09IE1Ub29uTWF0ZXJpYWxEZWJ1Z01vZGUuVVYsXG4gICAgICBPVVRMSU5FX1dJRFRIX1dPUkxEOiB0aGlzLl9vdXRsaW5lV2lkdGhNb2RlID09PSBNVG9vbk1hdGVyaWFsT3V0bGluZVdpZHRoTW9kZS5Xb3JsZENvb3JkaW5hdGVzLFxuICAgICAgT1VUTElORV9XSURUSF9TQ1JFRU46IHRoaXMuX291dGxpbmVXaWR0aE1vZGUgPT09IE1Ub29uTWF0ZXJpYWxPdXRsaW5lV2lkdGhNb2RlLlNjcmVlbkNvb3JkaW5hdGVzLFxuICAgICAgT1VUTElORV9DT0xPUl9GSVhFRDogdGhpcy5fb3V0bGluZUNvbG9yTW9kZSA9PT0gTVRvb25NYXRlcmlhbE91dGxpbmVDb2xvck1vZGUuRml4ZWRDb2xvcixcbiAgICAgIE9VVExJTkVfQ09MT1JfTUlYRUQ6IHRoaXMuX291dGxpbmVDb2xvck1vZGUgPT09IE1Ub29uTWF0ZXJpYWxPdXRsaW5lQ29sb3JNb2RlLk1peGVkTGlnaHRpbmcsXG4gICAgfTtcblxuICAgIC8vID09IHRleHR1cmUgZW5jb2RpbmdzID09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT1cbiAgICBjb25zdCBlbmNvZGluZ3MgPVxuICAgICAgKHRoaXMuc2hhZGVUZXh0dXJlICE9PSBudWxsXG4gICAgICAgID8gZ2V0VGV4ZWxEZWNvZGluZ0Z1bmN0aW9uKCdzaGFkZVRleHR1cmVUZXhlbFRvTGluZWFyJywgdGhpcy5zaGFkZVRleHR1cmUuZW5jb2RpbmcpICsgJ1xcbidcbiAgICAgICAgOiAnJykgK1xuICAgICAgKHRoaXMuc3BoZXJlQWRkICE9PSBudWxsXG4gICAgICAgID8gZ2V0VGV4ZWxEZWNvZGluZ0Z1bmN0aW9uKCdzcGhlcmVBZGRUZXhlbFRvTGluZWFyJywgdGhpcy5zcGhlcmVBZGQuZW5jb2RpbmcpICsgJ1xcbidcbiAgICAgICAgOiAnJykgK1xuICAgICAgKHRoaXMucmltVGV4dHVyZSAhPT0gbnVsbFxuICAgICAgICA/IGdldFRleGVsRGVjb2RpbmdGdW5jdGlvbigncmltVGV4dHVyZVRleGVsVG9MaW5lYXInLCB0aGlzLnJpbVRleHR1cmUuZW5jb2RpbmcpICsgJ1xcbidcbiAgICAgICAgOiAnJyk7XG5cbiAgICAvLyA9PSBnZW5lcmF0ZSBzaGFkZXIgY29kZSA9PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09XG4gICAgdGhpcy52ZXJ0ZXhTaGFkZXIgPSB2ZXJ0ZXhTaGFkZXI7XG4gICAgdGhpcy5mcmFnbWVudFNoYWRlciA9IGVuY29kaW5ncyArIGZyYWdtZW50U2hhZGVyO1xuXG4gICAgLy8gPT0gc2V0IG5lZWRzVXBkYXRlIGZsYWcgPT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PVxuICAgIHRoaXMubmVlZHNVcGRhdGUgPSB0cnVlO1xuICB9XG5cbiAgcHJpdmF0ZSBfdXBkYXRlQ3VsbEZhY2UoKTogdm9pZCB7XG4gICAgaWYgKCF0aGlzLmlzT3V0bGluZSkge1xuICAgICAgaWYgKHRoaXMuY3VsbE1vZGUgPT09IE1Ub29uTWF0ZXJpYWxDdWxsTW9kZS5PZmYpIHtcbiAgICAgICAgdGhpcy5zaWRlID0gVEhSRUUuRG91YmxlU2lkZTtcbiAgICAgIH0gZWxzZSBpZiAodGhpcy5jdWxsTW9kZSA9PT0gTVRvb25NYXRlcmlhbEN1bGxNb2RlLkZyb250KSB7XG4gICAgICAgIHRoaXMuc2lkZSA9IFRIUkVFLkJhY2tTaWRlO1xuICAgICAgfSBlbHNlIGlmICh0aGlzLmN1bGxNb2RlID09PSBNVG9vbk1hdGVyaWFsQ3VsbE1vZGUuQmFjaykge1xuICAgICAgICB0aGlzLnNpZGUgPSBUSFJFRS5Gcm9udFNpZGU7XG4gICAgICB9XG4gICAgfSBlbHNlIHtcbiAgICAgIGlmICh0aGlzLm91dGxpbmVDdWxsTW9kZSA9PT0gTVRvb25NYXRlcmlhbEN1bGxNb2RlLk9mZikge1xuICAgICAgICB0aGlzLnNpZGUgPSBUSFJFRS5Eb3VibGVTaWRlO1xuICAgICAgfSBlbHNlIGlmICh0aGlzLm91dGxpbmVDdWxsTW9kZSA9PT0gTVRvb25NYXRlcmlhbEN1bGxNb2RlLkZyb250KSB7XG4gICAgICAgIHRoaXMuc2lkZSA9IFRIUkVFLkJhY2tTaWRlO1xuICAgICAgfSBlbHNlIGlmICh0aGlzLm91dGxpbmVDdWxsTW9kZSA9PT0gTVRvb25NYXRlcmlhbEN1bGxNb2RlLkJhY2spIHtcbiAgICAgICAgdGhpcy5zaWRlID0gVEhSRUUuRnJvbnRTaWRlO1xuICAgICAgfVxuICAgIH1cbiAgfVxufVxuIiwiLyogdHNsaW50OmRpc2FibGU6bWVtYmVyLW9yZGVyaW5nICovXG5cbmltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcbmltcG9ydCB2ZXJ0ZXhTaGFkZXIgZnJvbSAnLi9zaGFkZXJzL3VubGl0LnZlcnQnO1xuaW1wb3J0IGZyYWdtZW50U2hhZGVyIGZyb20gJy4vc2hhZGVycy91bmxpdC5mcmFnJztcblxuZXhwb3J0IGludGVyZmFjZSBWUk1VbmxpdE1hdGVyaWFsUGFyYW1ldGVycyBleHRlbmRzIFRIUkVFLlNoYWRlck1hdGVyaWFsUGFyYW1ldGVycyB7XG4gIGN1dG9mZj86IG51bWJlcjsgLy8gX0N1dG9mZlxuICBtYXA/OiBUSFJFRS5UZXh0dXJlOyAvLyBfTWFpblRleFxuICBtYWluVGV4PzogVEhSRUUuVGV4dHVyZTsgLy8gX01haW5UZXggKHdpbGwgYmUgcmVuYW1lZCB0byBtYXApXG4gIC8vIGVzbGludC1kaXNhYmxlLW5leHQtbGluZSBAdHlwZXNjcmlwdC1lc2xpbnQvbmFtaW5nLWNvbnZlbnRpb25cbiAgbWFpblRleF9TVD86IFRIUkVFLlZlY3RvcjQ7IC8vIF9NYWluVGV4X1NUXG5cbiAgcmVuZGVyVHlwZT86IFZSTVVubGl0TWF0ZXJpYWxSZW5kZXJUeXBlIHwgbnVtYmVyO1xufVxuXG5leHBvcnQgZW51bSBWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZSB7XG4gIE9wYXF1ZSxcbiAgQ3V0b3V0LFxuICBUcmFuc3BhcmVudCxcbiAgVHJhbnNwYXJlbnRXaXRoWldyaXRlLFxufVxuXG4vKipcbiAqIFRoaXMgaXMgYSBtYXRlcmlhbCB0aGF0IGlzIGFuIGVxdWl2YWxlbnQgb2YgXCJWUk0vVW5saXQqKipcIiBvbiBWUk0gc3BlYywgdGhvc2UgbWF0ZXJpYWxzIGFyZSBhbHJlYWR5IGtpbmRhIGRlcHJlY2F0ZWQgdGhvdWdoLi4uXG4gKi9cbmV4cG9ydCBjbGFzcyBWUk1VbmxpdE1hdGVyaWFsIGV4dGVuZHMgVEhSRUUuU2hhZGVyTWF0ZXJpYWwge1xuICAvKipcbiAgICogUmVhZG9ubHkgYm9vbGVhbiB0aGF0IGluZGljYXRlcyB0aGlzIGlzIGEgW1tWUk1VbmxpdE1hdGVyaWFsXV0uXG4gICAqL1xuICBwdWJsaWMgcmVhZG9ubHkgaXNWUk1VbmxpdE1hdGVyaWFsOiBib29sZWFuID0gdHJ1ZTtcblxuICBwdWJsaWMgY3V0b2ZmID0gMC41O1xuICBwdWJsaWMgbWFwOiBUSFJFRS5UZXh0dXJlIHwgbnVsbCA9IG51bGw7IC8vIF9NYWluVGV4XG4gIC8vIGVzbGludC1kaXNhYmxlLW5leHQtbGluZSBAdHlwZXNjcmlwdC1lc2xpbnQvbmFtaW5nLWNvbnZlbnRpb25cbiAgcHVibGljIG1haW5UZXhfU1QgPSBuZXcgVEhSRUUuVmVjdG9yNCgwLjAsIDAuMCwgMS4wLCAxLjApOyAvLyBfTWFpblRleF9TVFxuICBwcml2YXRlIF9yZW5kZXJUeXBlID0gVlJNVW5saXRNYXRlcmlhbFJlbmRlclR5cGUuT3BhcXVlO1xuXG4gIHB1YmxpYyBzaG91bGRBcHBseVVuaWZvcm1zID0gdHJ1ZTsgLy8gd2hlbiB0aGlzIGlzIHRydWUsIGFwcGx5VW5pZm9ybXMgZWZmZWN0c1xuXG4gIGNvbnN0cnVjdG9yKHBhcmFtZXRlcnM/OiBWUk1VbmxpdE1hdGVyaWFsUGFyYW1ldGVycykge1xuICAgIHN1cGVyKCk7XG5cbiAgICBpZiAocGFyYW1ldGVycyA9PT0gdW5kZWZpbmVkKSB7XG4gICAgICBwYXJhbWV0ZXJzID0ge307XG4gICAgfVxuXG4gICAgLy8gPT0gZW5hYmxpbmcgYnVuY2ggb2Ygc3R1ZmYgPT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PVxuICAgIHBhcmFtZXRlcnMuZm9nID0gdHJ1ZTtcbiAgICBwYXJhbWV0ZXJzLmNsaXBwaW5nID0gdHJ1ZTtcblxuICAgIHBhcmFtZXRlcnMuc2tpbm5pbmcgPSBwYXJhbWV0ZXJzLnNraW5uaW5nIHx8IGZhbHNlO1xuICAgIHBhcmFtZXRlcnMubW9ycGhUYXJnZXRzID0gcGFyYW1ldGVycy5tb3JwaFRhcmdldHMgfHwgZmFsc2U7XG4gICAgcGFyYW1ldGVycy5tb3JwaE5vcm1hbHMgPSBwYXJhbWV0ZXJzLm1vcnBoTm9ybWFscyB8fCBmYWxzZTtcblxuICAgIC8vID09IHVuaWZvcm1zID09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT1cbiAgICBwYXJhbWV0ZXJzLnVuaWZvcm1zID0gVEhSRUUuVW5pZm9ybXNVdGlscy5tZXJnZShbXG4gICAgICBUSFJFRS5Vbmlmb3Jtc0xpYi5jb21tb24sIC8vIG1hcFxuICAgICAgVEhSRUUuVW5pZm9ybXNMaWIuZm9nLFxuICAgICAge1xuICAgICAgICBjdXRvZmY6IHsgdmFsdWU6IDAuNSB9LFxuICAgICAgICAvLyBlc2xpbnQtZGlzYWJsZS1uZXh0LWxpbmUgQHR5cGVzY3JpcHQtZXNsaW50L25hbWluZy1jb252ZW50aW9uXG4gICAgICAgIG1haW5UZXhfU1Q6IHsgdmFsdWU6IG5ldyBUSFJFRS5WZWN0b3I0KDAuMCwgMC4wLCAxLjAsIDEuMCkgfSxcbiAgICAgIH0sXG4gICAgXSk7XG5cbiAgICAvLyA9PSBmaW5hbGx5IGNvbXBpbGUgdGhlIHNoYWRlciBwcm9ncmFtID09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09XG4gICAgdGhpcy5zZXRWYWx1ZXMocGFyYW1ldGVycyk7XG5cbiAgICAvLyA9PSB1cGRhdGUgc2hhZGVyIHN0dWZmID09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09XG4gICAgdGhpcy5fdXBkYXRlU2hhZGVyQ29kZSgpO1xuICAgIHRoaXMuX2FwcGx5VW5pZm9ybXMoKTtcbiAgfVxuXG4gIGdldCBtYWluVGV4KCk6IFRIUkVFLlRleHR1cmUgfCBudWxsIHtcbiAgICByZXR1cm4gdGhpcy5tYXA7XG4gIH1cblxuICBzZXQgbWFpblRleCh0OiBUSFJFRS5UZXh0dXJlIHwgbnVsbCkge1xuICAgIHRoaXMubWFwID0gdDtcbiAgfVxuXG4gIGdldCByZW5kZXJUeXBlKCk6IFZSTVVubGl0TWF0ZXJpYWxSZW5kZXJUeXBlIHtcbiAgICByZXR1cm4gdGhpcy5fcmVuZGVyVHlwZTtcbiAgfVxuXG4gIHNldCByZW5kZXJUeXBlKHQ6IFZSTVVubGl0TWF0ZXJpYWxSZW5kZXJUeXBlKSB7XG4gICAgdGhpcy5fcmVuZGVyVHlwZSA9IHQ7XG5cbiAgICB0aGlzLmRlcHRoV3JpdGUgPSB0aGlzLl9yZW5kZXJUeXBlICE9PSBWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZS5UcmFuc3BhcmVudDtcbiAgICB0aGlzLnRyYW5zcGFyZW50ID1cbiAgICAgIHRoaXMuX3JlbmRlclR5cGUgPT09IFZSTVVubGl0TWF0ZXJpYWxSZW5kZXJUeXBlLlRyYW5zcGFyZW50IHx8XG4gICAgICB0aGlzLl9yZW5kZXJUeXBlID09PSBWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZS5UcmFuc3BhcmVudFdpdGhaV3JpdGU7XG4gICAgdGhpcy5fdXBkYXRlU2hhZGVyQ29kZSgpO1xuICB9XG5cbiAgLyoqXG4gICAqIFVwZGF0ZSB0aGlzIG1hdGVyaWFsLlxuICAgKiBVc3VhbGx5IHRoaXMgd2lsbCBiZSBjYWxsZWQgdmlhIFtbVlJNLnVwZGF0ZV1dIHNvIHlvdSBkb24ndCBoYXZlIHRvIGNhbGwgdGhpcyBtYW51YWxseS5cbiAgICpcbiAgICogQHBhcmFtIGRlbHRhIGRlbHRhVGltZSBzaW5jZSBsYXN0IHVwZGF0ZVxuICAgKi9cbiAgcHVibGljIHVwZGF0ZVZSTU1hdGVyaWFscyhkZWx0YTogbnVtYmVyKTogdm9pZCB7XG4gICAgdGhpcy5fYXBwbHlVbmlmb3JtcygpO1xuICB9XG5cbiAgcHVibGljIGNvcHkoc291cmNlOiB0aGlzKTogdGhpcyB7XG4gICAgc3VwZXIuY29weShzb3VyY2UpO1xuXG4gICAgLy8gPT0gY29weSBtZW1iZXJzID09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PVxuICAgIHRoaXMuY3V0b2ZmID0gc291cmNlLmN1dG9mZjtcbiAgICB0aGlzLm1hcCA9IHNvdXJjZS5tYXA7XG4gICAgdGhpcy5tYWluVGV4X1NULmNvcHkoc291cmNlLm1haW5UZXhfU1QpO1xuICAgIHRoaXMucmVuZGVyVHlwZSA9IHNvdXJjZS5yZW5kZXJUeXBlO1xuXG4gICAgcmV0dXJuIHRoaXM7XG4gIH1cblxuICAvKipcbiAgICogQXBwbHkgdXBkYXRlZCB1bmlmb3JtIHZhcmlhYmxlcy5cbiAgICovXG4gIHByaXZhdGUgX2FwcGx5VW5pZm9ybXMoKTogdm9pZCB7XG4gICAgaWYgKCF0aGlzLnNob3VsZEFwcGx5VW5pZm9ybXMpIHtcbiAgICAgIHJldHVybjtcbiAgICB9XG4gICAgdGhpcy5zaG91bGRBcHBseVVuaWZvcm1zID0gZmFsc2U7XG5cbiAgICB0aGlzLnVuaWZvcm1zLmN1dG9mZi52YWx1ZSA9IHRoaXMuY3V0b2ZmO1xuICAgIHRoaXMudW5pZm9ybXMubWFwLnZhbHVlID0gdGhpcy5tYXA7XG4gICAgdGhpcy51bmlmb3Jtcy5tYWluVGV4X1NULnZhbHVlLmNvcHkodGhpcy5tYWluVGV4X1NUKTtcbiAgfVxuXG4gIHByaXZhdGUgX3VwZGF0ZVNoYWRlckNvZGUoKTogdm9pZCB7XG4gICAgdGhpcy5kZWZpbmVzID0ge1xuICAgICAgUkVOREVSVFlQRV9PUEFRVUU6IHRoaXMuX3JlbmRlclR5cGUgPT09IFZSTVVubGl0TWF0ZXJpYWxSZW5kZXJUeXBlLk9wYXF1ZSxcbiAgICAgIFJFTkRFUlRZUEVfQ1VUT1VUOiB0aGlzLl9yZW5kZXJUeXBlID09PSBWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZS5DdXRvdXQsXG4gICAgICBSRU5ERVJUWVBFX1RSQU5TUEFSRU5UOlxuICAgICAgICB0aGlzLl9yZW5kZXJUeXBlID09PSBWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZS5UcmFuc3BhcmVudCB8fFxuICAgICAgICB0aGlzLl9yZW5kZXJUeXBlID09PSBWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZS5UcmFuc3BhcmVudFdpdGhaV3JpdGUsXG4gICAgfTtcblxuICAgIHRoaXMudmVydGV4U2hhZGVyID0gdmVydGV4U2hhZGVyO1xuICAgIHRoaXMuZnJhZ21lbnRTaGFkZXIgPSBmcmFnbWVudFNoYWRlcjtcblxuICAgIC8vID09IHNldCBuZWVkc1VwZGF0ZSBmbGFnID09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT1cbiAgICB0aGlzLm5lZWRzVXBkYXRlID0gdHJ1ZTtcbiAgfVxufVxuIiwiaW1wb3J0ICogYXMgVEhSRUUgZnJvbSAndGhyZWUnO1xuaW1wb3J0IHsgR0xURiB9IGZyb20gJ3RocmVlL2V4YW1wbGVzL2pzbS9sb2FkZXJzL0dMVEZMb2FkZXInO1xuaW1wb3J0IHsgR0xURlNjaGVtYSwgVlJNU2NoZW1hIH0gZnJvbSAnLi4vdHlwZXMnO1xuaW1wb3J0IHsgZ2x0ZkV4dHJhY3RQcmltaXRpdmVzRnJvbU5vZGVzIH0gZnJvbSAnLi4vdXRpbHMvZ2x0ZkV4dHJhY3RQcmltaXRpdmVzRnJvbU5vZGUnO1xuaW1wb3J0IHsgTVRvb25NYXRlcmlhbCwgTVRvb25NYXRlcmlhbE91dGxpbmVXaWR0aE1vZGUgfSBmcm9tICcuL01Ub29uTWF0ZXJpYWwnO1xuaW1wb3J0IHsgVlJNVW5saXRNYXRlcmlhbCwgVlJNVW5saXRNYXRlcmlhbFJlbmRlclR5cGUgfSBmcm9tICcuL1ZSTVVubGl0TWF0ZXJpYWwnO1xuXG4vKipcbiAqIE9wdGlvbnMgZm9yIGEgW1tWUk1NYXRlcmlhbEltcG9ydGVyXV0gaW5zdGFuY2UuXG4gKi9cbmV4cG9ydCBpbnRlcmZhY2UgVlJNTWF0ZXJpYWxJbXBvcnRlck9wdGlvbnMge1xuICAvKipcbiAgICogU3BlY2lmeSB0aGUgZW5jb2Rpbmcgb2YgaW5wdXQgdW5pZm9ybSBjb2xvcnMgYW5kIHRleHR1cmVzLlxuICAgKlxuICAgKiBXaGVuIHlvdXIgYHJlbmRlcmVyLm91dHB1dEVuY29kaW5nYCBpcyBgVEhSRUUuTGluZWFyRW5jb2RpbmdgLCB1c2UgYFRIUkVFLkxpbmVhckVuY29kaW5nYC5cbiAgICogV2hlbiB5b3VyIGByZW5kZXJlci5vdXRwdXRFbmNvZGluZ2AgaXMgYFRIUkVFLnNSR0JFbmNvZGluZ2AsIHVzZSBgVEhSRUUuc1JHQkVuY29kaW5nYC5cbiAgICpcbiAgICogVGhlIGltcG9ydGVyIHdpbGwgdXNlIGBUSFJFRS5MaW5lYXJFbmNvZGluZ2AgaWYgdGhpcyBvcHRpb24gaXNuJ3Qgc3BlY2lmaWVkLlxuICAgKlxuICAgKiBTZWUgYWxzbzogaHR0cHM6Ly90aHJlZWpzLm9yZy9kb2NzLyNhcGkvZW4vcmVuZGVyZXJzL1dlYkdMUmVuZGVyZXIub3V0cHV0RW5jb2RpbmdcbiAgICovXG4gIGVuY29kaW5nPzogVEhSRUUuVGV4dHVyZUVuY29kaW5nO1xuXG4gIC8qKlxuICAgKiBBIGZ1bmN0aW9uIHRoYXQgcmV0dXJucyBhIGBQcm9taXNlYCBvZiBlbnZpcm9ubWVudCBtYXAgdGV4dHVyZS5cbiAgICogVGhlIGltcG9ydGVyIHdpbGwgYXR0ZW1wdCB0byBjYWxsIHRoaXMgZnVuY3Rpb24gd2hlbiBpdCBoYXZlIHRvIHVzZSBhbiBlbnZtYXAuXG4gICAqL1xuICByZXF1ZXN0RW52TWFwPzogKCkgPT4gUHJvbWlzZTxUSFJFRS5UZXh0dXJlIHwgbnVsbD47XG59XG5cbi8qKlxuICogQW4gaW1wb3J0ZXIgdGhhdCBpbXBvcnRzIFZSTSBtYXRlcmlhbHMgZnJvbSBhIFZSTSBleHRlbnNpb24gb2YgYSBHTFRGLlxuICovXG5leHBvcnQgY2xhc3MgVlJNTWF0ZXJpYWxJbXBvcnRlciB7XG4gIHByaXZhdGUgcmVhZG9ubHkgX2VuY29kaW5nOiBUSFJFRS5UZXh0dXJlRW5jb2Rpbmc7XG4gIHByaXZhdGUgcmVhZG9ubHkgX3JlcXVlc3RFbnZNYXA/OiAoKSA9PiBQcm9taXNlPFRIUkVFLlRleHR1cmUgfCBudWxsPjtcblxuICAvKipcbiAgICogQ3JlYXRlIGEgbmV3IFZSTU1hdGVyaWFsSW1wb3J0ZXIuXG4gICAqXG4gICAqIEBwYXJhbSBvcHRpb25zIE9wdGlvbnMgb2YgdGhlIFZSTU1hdGVyaWFsSW1wb3J0ZXJcbiAgICovXG4gIGNvbnN0cnVjdG9yKG9wdGlvbnM6IFZSTU1hdGVyaWFsSW1wb3J0ZXJPcHRpb25zID0ge30pIHtcbiAgICB0aGlzLl9lbmNvZGluZyA9IG9wdGlvbnMuZW5jb2RpbmcgfHwgVEhSRUUuTGluZWFyRW5jb2Rpbmc7XG4gICAgaWYgKHRoaXMuX2VuY29kaW5nICE9PSBUSFJFRS5MaW5lYXJFbmNvZGluZyAmJiB0aGlzLl9lbmNvZGluZyAhPT0gVEhSRUUuc1JHQkVuY29kaW5nKSB7XG4gICAgICBjb25zb2xlLndhcm4oXG4gICAgICAgICdUaGUgc3BlY2lmaWVkIGNvbG9yIGVuY29kaW5nIG1pZ2h0IG5vdCB3b3JrIHByb3Blcmx5IHdpdGggVlJNTWF0ZXJpYWxJbXBvcnRlci4gWW91IG1pZ2h0IHdhbnQgdG8gdXNlIFRIUkVFLnNSR0JFbmNvZGluZyBpbnN0ZWFkLicsXG4gICAgICApO1xuICAgIH1cblxuICAgIHRoaXMuX3JlcXVlc3RFbnZNYXAgPSBvcHRpb25zLnJlcXVlc3RFbnZNYXA7XG4gIH1cblxuICAvKipcbiAgICogQ29udmVydCBhbGwgdGhlIG1hdGVyaWFscyBvZiBnaXZlbiBHTFRGIGJhc2VkIG9uIFZSTSBleHRlbnNpb24gZmllbGQgYG1hdGVyaWFsUHJvcGVydGllc2AuXG4gICAqXG4gICAqIEBwYXJhbSBnbHRmIEEgcGFyc2VkIHJlc3VsdCBvZiBHTFRGIHRha2VuIGZyb20gR0xURkxvYWRlclxuICAgKi9cbiAgcHVibGljIGFzeW5jIGNvbnZlcnRHTFRGTWF0ZXJpYWxzKGdsdGY6IEdMVEYpOiBQcm9taXNlPFRIUkVFLk1hdGVyaWFsW10gfCBudWxsPiB7XG4gICAgY29uc3QgdnJtRXh0OiBWUk1TY2hlbWEuVlJNIHwgdW5kZWZpbmVkID0gZ2x0Zi5wYXJzZXIuanNvbi5leHRlbnNpb25zPy5WUk07XG4gICAgaWYgKCF2cm1FeHQpIHtcbiAgICAgIHJldHVybiBudWxsO1xuICAgIH1cblxuICAgIGNvbnN0IG1hdGVyaWFsUHJvcGVydGllczogVlJNU2NoZW1hLk1hdGVyaWFsW10gfCB1bmRlZmluZWQgPSB2cm1FeHQubWF0ZXJpYWxQcm9wZXJ0aWVzO1xuICAgIGlmICghbWF0ZXJpYWxQcm9wZXJ0aWVzKSB7XG4gICAgICByZXR1cm4gbnVsbDtcbiAgICB9XG5cbiAgICBjb25zdCBub2RlUHJpbWl0aXZlc01hcCA9IGF3YWl0IGdsdGZFeHRyYWN0UHJpbWl0aXZlc0Zyb21Ob2RlcyhnbHRmKTtcbiAgICBjb25zdCBtYXRlcmlhbExpc3Q6IHsgW3ZybU1hdGVyaWFsSW5kZXg6IG51bWJlcl06IHsgc3VyZmFjZTogVEhSRUUuTWF0ZXJpYWw7IG91dGxpbmU/OiBUSFJFRS5NYXRlcmlhbCB9IH0gPSB7fTtcbiAgICBjb25zdCBtYXRlcmlhbHM6IFRIUkVFLk1hdGVyaWFsW10gPSBbXTsgLy8gcmVzdWx0XG5cbiAgICBhd2FpdCBQcm9taXNlLmFsbChcbiAgICAgIEFycmF5LmZyb20obm9kZVByaW1pdGl2ZXNNYXAuZW50cmllcygpKS5tYXAoYXN5bmMgKFtub2RlSW5kZXgsIHByaW1pdGl2ZXNdKSA9PiB7XG4gICAgICAgIGNvbnN0IHNjaGVtYU5vZGU6IEdMVEZTY2hlbWEuTm9kZSA9IGdsdGYucGFyc2VyLmpzb24ubm9kZXNbbm9kZUluZGV4XTtcbiAgICAgICAgY29uc3Qgc2NoZW1hTWVzaDogR0xURlNjaGVtYS5NZXNoID0gZ2x0Zi5wYXJzZXIuanNvbi5tZXNoZXNbc2NoZW1hTm9kZS5tZXNoIV07XG5cbiAgICAgICAgYXdhaXQgUHJvbWlzZS5hbGwoXG4gICAgICAgICAgcHJpbWl0aXZlcy5tYXAoYXN5bmMgKHByaW1pdGl2ZSwgcHJpbWl0aXZlSW5kZXgpID0+IHtcbiAgICAgICAgICAgIGNvbnN0IHNjaGVtYVByaW1pdGl2ZSA9IHNjaGVtYU1lc2gucHJpbWl0aXZlc1twcmltaXRpdmVJbmRleF07XG5cbiAgICAgICAgICAgIC8vIHNvbWUgZ2xURiBtaWdodCBoYXZlIGJvdGggYG5vZGUubWVzaGAgYW5kIGBub2RlLmNoaWxkcmVuYCBhdCBvbmNlXG4gICAgICAgICAgICAvLyBhbmQgR0xURkxvYWRlciBoYW5kbGVzIGJvdGggbWVzaCBwcmltaXRpdmVzIGFuZCBcImNoaWxkcmVuXCIgaW4gZ2xURiBhcyBcImNoaWxkcmVuXCIgaW4gVEhSRUVcbiAgICAgICAgICAgIC8vIEl0IHNlZW1zIEdMVEZMb2FkZXIgaGFuZGxlcyBwcmltaXRpdmVzIGZpcnN0IHRoZW4gaGFuZGxlcyBcImNoaWxkcmVuXCIgaW4gZ2xURiAoaXQncyBsdWNreSEpXG4gICAgICAgICAgICAvLyBzbyB3ZSBzaG91bGQgaWdub3JlIChwcmltaXRpdmVzLmxlbmd0aCl0aCBhbmQgZm9sbG93aW5nIGNoaWxkcmVuIG9mIGBtZXNoLmNoaWxkcmVuYFxuICAgICAgICAgICAgLy8gVE9ETzogc2FuaXRpemUgdGhpcyBhZnRlciBHTFRGTG9hZGVyIHBsdWdpbiBzeXN0ZW0gZ2V0cyBpbnRyb2R1Y2VkIDogaHR0cHM6Ly9naXRodWIuY29tL21yZG9vYi90aHJlZS5qcy9wdWxsLzE4NDIxXG4gICAgICAgICAgICBpZiAoIXNjaGVtYVByaW1pdGl2ZSkge1xuICAgICAgICAgICAgICByZXR1cm47XG4gICAgICAgICAgICB9XG5cbiAgICAgICAgICAgIGNvbnN0IHByaW1pdGl2ZUdlb21ldHJ5ID0gcHJpbWl0aXZlLmdlb21ldHJ5O1xuICAgICAgICAgICAgY29uc3QgcHJpbWl0aXZlVmVydGljZXMgPSBwcmltaXRpdmVHZW9tZXRyeS5pbmRleFxuICAgICAgICAgICAgICA/IHByaW1pdGl2ZUdlb21ldHJ5LmluZGV4LmNvdW50XG4gICAgICAgICAgICAgIDogcHJpbWl0aXZlR2VvbWV0cnkuYXR0cmlidXRlcy5wb3NpdGlvbi5jb3VudCAvIDM7XG5cbiAgICAgICAgICAgIC8vIGlmIHByaW1pdGl2ZXMgbWF0ZXJpYWwgaXMgbm90IGFuIGFycmF5LCBtYWtlIGl0IGFuIGFycmF5XG4gICAgICAgICAgICBpZiAoIUFycmF5LmlzQXJyYXkocHJpbWl0aXZlLm1hdGVyaWFsKSkge1xuICAgICAgICAgICAgICBwcmltaXRpdmUubWF0ZXJpYWwgPSBbcHJpbWl0aXZlLm1hdGVyaWFsXTtcbiAgICAgICAgICAgICAgcHJpbWl0aXZlR2VvbWV0cnkuYWRkR3JvdXAoMCwgcHJpbWl0aXZlVmVydGljZXMsIDApO1xuICAgICAgICAgICAgfVxuXG4gICAgICAgICAgICAvLyBjcmVhdGUgLyBwdXNoIHRvIGNhY2hlIChvciBwb3AgZnJvbSBjYWNoZSkgdnJtIG1hdGVyaWFsc1xuICAgICAgICAgICAgY29uc3QgdnJtTWF0ZXJpYWxJbmRleCA9IHNjaGVtYVByaW1pdGl2ZS5tYXRlcmlhbCE7XG5cbiAgICAgICAgICAgIGxldCBwcm9wcyA9IG1hdGVyaWFsUHJvcGVydGllc1t2cm1NYXRlcmlhbEluZGV4XTtcbiAgICAgICAgICAgIGlmICghcHJvcHMpIHtcbiAgICAgICAgICAgICAgY29uc29sZS53YXJuKFxuICAgICAgICAgICAgICAgIGBWUk1NYXRlcmlhbEltcG9ydGVyOiBUaGVyZSBhcmUgbm8gbWF0ZXJpYWwgZGVmaW5pdGlvbiBmb3IgbWF0ZXJpYWwgIyR7dnJtTWF0ZXJpYWxJbmRleH0gb24gVlJNIGV4dGVuc2lvbi5gLFxuICAgICAgICAgICAgICApO1xuICAgICAgICAgICAgICBwcm9wcyA9IHsgc2hhZGVyOiAnVlJNX1VTRV9HTFRGU0hBREVSJyB9OyAvLyBmYWxsYmFja1xuICAgICAgICAgICAgfVxuXG4gICAgICAgICAgICBsZXQgdnJtTWF0ZXJpYWxzOiB7IHN1cmZhY2U6IFRIUkVFLk1hdGVyaWFsOyBvdXRsaW5lPzogVEhSRUUuTWF0ZXJpYWwgfTtcbiAgICAgICAgICAgIGlmIChtYXRlcmlhbExpc3RbdnJtTWF0ZXJpYWxJbmRleF0pIHtcbiAgICAgICAgICAgICAgdnJtTWF0ZXJpYWxzID0gbWF0ZXJpYWxMaXN0W3ZybU1hdGVyaWFsSW5kZXhdO1xuICAgICAgICAgICAgfSBlbHNlIHtcbiAgICAgICAgICAgICAgdnJtTWF0ZXJpYWxzID0gYXdhaXQgdGhpcy5jcmVhdGVWUk1NYXRlcmlhbHMocHJpbWl0aXZlLm1hdGVyaWFsWzBdLCBwcm9wcywgZ2x0Zik7XG4gICAgICAgICAgICAgIG1hdGVyaWFsTGlzdFt2cm1NYXRlcmlhbEluZGV4XSA9IHZybU1hdGVyaWFscztcblxuICAgICAgICAgICAgICBtYXRlcmlhbHMucHVzaCh2cm1NYXRlcmlhbHMuc3VyZmFjZSk7XG4gICAgICAgICAgICAgIGlmICh2cm1NYXRlcmlhbHMub3V0bGluZSkge1xuICAgICAgICAgICAgICAgIG1hdGVyaWFscy5wdXNoKHZybU1hdGVyaWFscy5vdXRsaW5lKTtcbiAgICAgICAgICAgICAgfVxuICAgICAgICAgICAgfVxuXG4gICAgICAgICAgICAvLyBzdXJmYWNlXG4gICAgICAgICAgICBwcmltaXRpdmUubWF0ZXJpYWxbMF0gPSB2cm1NYXRlcmlhbHMuc3VyZmFjZTtcblxuICAgICAgICAgICAgLy8gZW52bWFwXG4gICAgICAgICAgICBpZiAodGhpcy5fcmVxdWVzdEVudk1hcCAmJiAodnJtTWF0ZXJpYWxzLnN1cmZhY2UgYXMgYW55KS5pc01lc2hTdGFuZGFyZE1hdGVyaWFsKSB7XG4gICAgICAgICAgICAgIHRoaXMuX3JlcXVlc3RFbnZNYXAoKS50aGVuKChlbnZNYXApID0+IHtcbiAgICAgICAgICAgICAgICAodnJtTWF0ZXJpYWxzLnN1cmZhY2UgYXMgYW55KS5lbnZNYXAgPSBlbnZNYXA7XG4gICAgICAgICAgICAgICAgdnJtTWF0ZXJpYWxzLnN1cmZhY2UubmVlZHNVcGRhdGUgPSB0cnVlO1xuICAgICAgICAgICAgICB9KTtcbiAgICAgICAgICAgIH1cblxuICAgICAgICAgICAgLy8gcmVuZGVyIG9yZGVyXG4gICAgICAgICAgICBwcmltaXRpdmUucmVuZGVyT3JkZXIgPSBwcm9wcy5yZW5kZXJRdWV1ZSB8fCAyMDAwO1xuXG4gICAgICAgICAgICAvLyBvdXRsaW5lIChcIjIgcGFzcyBzaGFkaW5nIHVzaW5nIGdyb3Vwc1wiIHRyaWNrIGhlcmUpXG4gICAgICAgICAgICBpZiAodnJtTWF0ZXJpYWxzLm91dGxpbmUpIHtcbiAgICAgICAgICAgICAgcHJpbWl0aXZlLm1hdGVyaWFsWzFdID0gdnJtTWF0ZXJpYWxzLm91dGxpbmU7XG4gICAgICAgICAgICAgIHByaW1pdGl2ZUdlb21ldHJ5LmFkZEdyb3VwKDAsIHByaW1pdGl2ZVZlcnRpY2VzLCAxKTtcbiAgICAgICAgICAgIH1cbiAgICAgICAgICB9KSxcbiAgICAgICAgKTtcbiAgICAgIH0pLFxuICAgICk7XG5cbiAgICByZXR1cm4gbWF0ZXJpYWxzO1xuICB9XG5cbiAgcHVibGljIGFzeW5jIGNyZWF0ZVZSTU1hdGVyaWFscyhcbiAgICBvcmlnaW5hbE1hdGVyaWFsOiBUSFJFRS5NYXRlcmlhbCxcbiAgICB2cm1Qcm9wczogVlJNU2NoZW1hLk1hdGVyaWFsLFxuICAgIGdsdGY6IEdMVEYsXG4gICk6IFByb21pc2U8e1xuICAgIHN1cmZhY2U6IFRIUkVFLk1hdGVyaWFsO1xuICAgIG91dGxpbmU/OiBUSFJFRS5NYXRlcmlhbDtcbiAgfT4ge1xuICAgIGxldCBuZXdTdXJmYWNlOiBUSFJFRS5NYXRlcmlhbCB8IHVuZGVmaW5lZDtcbiAgICBsZXQgbmV3T3V0bGluZTogVEhSRUUuTWF0ZXJpYWwgfCB1bmRlZmluZWQ7XG5cbiAgICBpZiAodnJtUHJvcHMuc2hhZGVyID09PSAnVlJNL01Ub29uJykge1xuICAgICAgY29uc3QgcGFyYW1zID0gYXdhaXQgdGhpcy5fZXh0cmFjdE1hdGVyaWFsUHJvcGVydGllcyhvcmlnaW5hbE1hdGVyaWFsLCB2cm1Qcm9wcywgZ2x0Zik7XG5cbiAgICAgIC8vIHdlIG5lZWQgdG8gZ2V0IHJpZCBvZiB0aGVzZSBwcm9wZXJ0aWVzXG4gICAgICBbJ3NyY0JsZW5kJywgJ2RzdEJsZW5kJywgJ2lzRmlyc3RTZXR1cCddLmZvckVhY2goKG5hbWUpID0+IHtcbiAgICAgICAgaWYgKHBhcmFtc1tuYW1lXSAhPT0gdW5kZWZpbmVkKSB7XG4gICAgICAgICAgZGVsZXRlIHBhcmFtc1tuYW1lXTtcbiAgICAgICAgfVxuICAgICAgfSk7XG5cbiAgICAgIC8vIHRoZXNlIHRleHR1cmVzIG11c3QgYmUgc1JHQiBFbmNvZGluZywgZGVwZW5kcyBvbiBjdXJyZW50IGNvbG9yc3BhY2VcbiAgICAgIFsnbWFpblRleCcsICdzaGFkZVRleHR1cmUnLCAnZW1pc3Npb25NYXAnLCAnc3BoZXJlQWRkJywgJ3JpbVRleHR1cmUnXS5mb3JFYWNoKChuYW1lKSA9PiB7XG4gICAgICAgIGlmIChwYXJhbXNbbmFtZV0gIT09IHVuZGVmaW5lZCkge1xuICAgICAgICAgIHBhcmFtc1tuYW1lXS5lbmNvZGluZyA9IHRoaXMuX2VuY29kaW5nO1xuICAgICAgICB9XG4gICAgICB9KTtcblxuICAgICAgLy8gc3BlY2lmeSB1bmlmb3JtIGNvbG9yIGVuY29kaW5nc1xuICAgICAgcGFyYW1zLmVuY29kaW5nID0gdGhpcy5fZW5jb2Rpbmc7XG5cbiAgICAgIC8vIGRvbmVcbiAgICAgIG5ld1N1cmZhY2UgPSBuZXcgTVRvb25NYXRlcmlhbChwYXJhbXMpO1xuXG4gICAgICAvLyBvdXRsaW5lXG4gICAgICBpZiAocGFyYW1zLm91dGxpbmVXaWR0aE1vZGUgIT09IE1Ub29uTWF0ZXJpYWxPdXRsaW5lV2lkdGhNb2RlLk5vbmUpIHtcbiAgICAgICAgcGFyYW1zLmlzT3V0bGluZSA9IHRydWU7XG4gICAgICAgIG5ld091dGxpbmUgPSBuZXcgTVRvb25NYXRlcmlhbChwYXJhbXMpO1xuICAgICAgfVxuICAgIH0gZWxzZSBpZiAodnJtUHJvcHMuc2hhZGVyID09PSAnVlJNL1VubGl0VGV4dHVyZScpIHtcbiAgICAgIC8vIHRoaXMgaXMgdmVyeSBsZWdhY3lcbiAgICAgIGNvbnN0IHBhcmFtcyA9IGF3YWl0IHRoaXMuX2V4dHJhY3RNYXRlcmlhbFByb3BlcnRpZXMob3JpZ2luYWxNYXRlcmlhbCwgdnJtUHJvcHMsIGdsdGYpO1xuICAgICAgcGFyYW1zLnJlbmRlclR5cGUgPSBWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZS5PcGFxdWU7XG4gICAgICBuZXdTdXJmYWNlID0gbmV3IFZSTVVubGl0TWF0ZXJpYWwocGFyYW1zKTtcbiAgICB9IGVsc2UgaWYgKHZybVByb3BzLnNoYWRlciA9PT0gJ1ZSTS9VbmxpdEN1dG91dCcpIHtcbiAgICAgIC8vIHRoaXMgaXMgdmVyeSBsZWdhY3lcbiAgICAgIGNvbnN0IHBhcmFtcyA9IGF3YWl0IHRoaXMuX2V4dHJhY3RNYXRlcmlhbFByb3BlcnRpZXMob3JpZ2luYWxNYXRlcmlhbCwgdnJtUHJvcHMsIGdsdGYpO1xuICAgICAgcGFyYW1zLnJlbmRlclR5cGUgPSBWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZS5DdXRvdXQ7XG4gICAgICBuZXdTdXJmYWNlID0gbmV3IFZSTVVubGl0TWF0ZXJpYWwocGFyYW1zKTtcbiAgICB9IGVsc2UgaWYgKHZybVByb3BzLnNoYWRlciA9PT0gJ1ZSTS9VbmxpdFRyYW5zcGFyZW50Jykge1xuICAgICAgLy8gdGhpcyBpcyB2ZXJ5IGxlZ2FjeVxuICAgICAgY29uc3QgcGFyYW1zID0gYXdhaXQgdGhpcy5fZXh0cmFjdE1hdGVyaWFsUHJvcGVydGllcyhvcmlnaW5hbE1hdGVyaWFsLCB2cm1Qcm9wcywgZ2x0Zik7XG4gICAgICBwYXJhbXMucmVuZGVyVHlwZSA9IFZSTVVubGl0TWF0ZXJpYWxSZW5kZXJUeXBlLlRyYW5zcGFyZW50O1xuICAgICAgbmV3U3VyZmFjZSA9IG5ldyBWUk1VbmxpdE1hdGVyaWFsKHBhcmFtcyk7XG4gICAgfSBlbHNlIGlmICh2cm1Qcm9wcy5zaGFkZXIgPT09ICdWUk0vVW5saXRUcmFuc3BhcmVudFpXcml0ZScpIHtcbiAgICAgIC8vIHRoaXMgaXMgdmVyeSBsZWdhY3lcbiAgICAgIGNvbnN0IHBhcmFtcyA9IGF3YWl0IHRoaXMuX2V4dHJhY3RNYXRlcmlhbFByb3BlcnRpZXMob3JpZ2luYWxNYXRlcmlhbCwgdnJtUHJvcHMsIGdsdGYpO1xuICAgICAgcGFyYW1zLnJlbmRlclR5cGUgPSBWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZS5UcmFuc3BhcmVudFdpdGhaV3JpdGU7XG4gICAgICBuZXdTdXJmYWNlID0gbmV3IFZSTVVubGl0TWF0ZXJpYWwocGFyYW1zKTtcbiAgICB9IGVsc2Uge1xuICAgICAgaWYgKHZybVByb3BzLnNoYWRlciAhPT0gJ1ZSTV9VU0VfR0xURlNIQURFUicpIHtcbiAgICAgICAgY29uc29sZS53YXJuKGBVbmtub3duIHNoYWRlciBkZXRlY3RlZDogXCIke3ZybVByb3BzLnNoYWRlcn1cImApO1xuICAgICAgICAvLyB0aGVuIHByZXN1bWUgYXMgVlJNX1VTRV9HTFRGU0hBREVSXG4gICAgICB9XG5cbiAgICAgIG5ld1N1cmZhY2UgPSB0aGlzLl9jb252ZXJ0R0xURk1hdGVyaWFsKG9yaWdpbmFsTWF0ZXJpYWwuY2xvbmUoKSk7XG4gICAgfVxuXG4gICAgbmV3U3VyZmFjZS5uYW1lID0gb3JpZ2luYWxNYXRlcmlhbC5uYW1lO1xuICAgIG5ld1N1cmZhY2UudXNlckRhdGEgPSBKU09OLnBhcnNlKEpTT04uc3RyaW5naWZ5KG9yaWdpbmFsTWF0ZXJpYWwudXNlckRhdGEpKTtcbiAgICBuZXdTdXJmYWNlLnVzZXJEYXRhLnZybU1hdGVyaWFsUHJvcGVydGllcyA9IHZybVByb3BzO1xuXG4gICAgaWYgKG5ld091dGxpbmUpIHtcbiAgICAgIG5ld091dGxpbmUubmFtZSA9IG9yaWdpbmFsTWF0ZXJpYWwubmFtZSArICcgKE91dGxpbmUpJztcbiAgICAgIG5ld091dGxpbmUudXNlckRhdGEgPSBKU09OLnBhcnNlKEpTT04uc3RyaW5naWZ5KG9yaWdpbmFsTWF0ZXJpYWwudXNlckRhdGEpKTtcbiAgICAgIG5ld091dGxpbmUudXNlckRhdGEudnJtTWF0ZXJpYWxQcm9wZXJ0aWVzID0gdnJtUHJvcHM7XG4gICAgfVxuXG4gICAgcmV0dXJuIHtcbiAgICAgIHN1cmZhY2U6IG5ld1N1cmZhY2UsXG4gICAgICBvdXRsaW5lOiBuZXdPdXRsaW5lLFxuICAgIH07XG4gIH1cblxuICBwcml2YXRlIF9yZW5hbWVNYXRlcmlhbFByb3BlcnR5KG5hbWU6IHN0cmluZyk6IHN0cmluZyB7XG4gICAgaWYgKG5hbWVbMF0gIT09ICdfJykge1xuICAgICAgY29uc29sZS53YXJuKGBWUk1NYXRlcmlhbHM6IEdpdmVuIHByb3BlcnR5IG5hbWUgXCIke25hbWV9XCIgbWlnaHQgYmUgaW52YWxpZGApO1xuICAgICAgcmV0dXJuIG5hbWU7XG4gICAgfVxuICAgIG5hbWUgPSBuYW1lLnN1YnN0cmluZygxKTtcblxuICAgIGlmICghL1tBLVpdLy50ZXN0KG5hbWVbMF0pKSB7XG4gICAgICBjb25zb2xlLndhcm4oYFZSTU1hdGVyaWFsczogR2l2ZW4gcHJvcGVydHkgbmFtZSBcIiR7bmFtZX1cIiBtaWdodCBiZSBpbnZhbGlkYCk7XG4gICAgICByZXR1cm4gbmFtZTtcbiAgICB9XG4gICAgcmV0dXJuIG5hbWVbMF0udG9Mb3dlckNhc2UoKSArIG5hbWUuc3Vic3RyaW5nKDEpO1xuICB9XG5cbiAgcHJpdmF0ZSBfY29udmVydEdMVEZNYXRlcmlhbChtYXRlcmlhbDogVEhSRUUuTWF0ZXJpYWwpOiBUSFJFRS5NYXRlcmlhbCB7XG4gICAgaWYgKChtYXRlcmlhbCBhcyBhbnkpLmlzTWVzaFN0YW5kYXJkTWF0ZXJpYWwpIHtcbiAgICAgIGNvbnN0IG10bCA9IG1hdGVyaWFsIGFzIFRIUkVFLk1lc2hTdGFuZGFyZE1hdGVyaWFsO1xuXG4gICAgICBpZiAobXRsLm1hcCkge1xuICAgICAgICBtdGwubWFwLmVuY29kaW5nID0gdGhpcy5fZW5jb2Rpbmc7XG4gICAgICB9XG4gICAgICBpZiAobXRsLmVtaXNzaXZlTWFwKSB7XG4gICAgICAgIG10bC5lbWlzc2l2ZU1hcC5lbmNvZGluZyA9IHRoaXMuX2VuY29kaW5nO1xuICAgICAgfVxuXG4gICAgICBpZiAodGhpcy5fZW5jb2RpbmcgPT09IFRIUkVFLkxpbmVhckVuY29kaW5nKSB7XG4gICAgICAgIG10bC5jb2xvci5jb252ZXJ0TGluZWFyVG9TUkdCKCk7XG4gICAgICAgIG10bC5lbWlzc2l2ZS5jb252ZXJ0TGluZWFyVG9TUkdCKCk7XG4gICAgICB9XG4gICAgfVxuXG4gICAgaWYgKChtYXRlcmlhbCBhcyBhbnkpLmlzTWVzaEJhc2ljTWF0ZXJpYWwpIHtcbiAgICAgIGNvbnN0IG10bCA9IG1hdGVyaWFsIGFzIFRIUkVFLk1lc2hCYXNpY01hdGVyaWFsO1xuXG4gICAgICBpZiAobXRsLm1hcCkge1xuICAgICAgICBtdGwubWFwLmVuY29kaW5nID0gdGhpcy5fZW5jb2Rpbmc7XG4gICAgICB9XG5cbiAgICAgIGlmICh0aGlzLl9lbmNvZGluZyA9PT0gVEhSRUUuTGluZWFyRW5jb2RpbmcpIHtcbiAgICAgICAgbXRsLmNvbG9yLmNvbnZlcnRMaW5lYXJUb1NSR0IoKTtcbiAgICAgIH1cbiAgICB9XG5cbiAgICByZXR1cm4gbWF0ZXJpYWw7XG4gIH1cblxuICBwcml2YXRlIF9leHRyYWN0TWF0ZXJpYWxQcm9wZXJ0aWVzKFxuICAgIG9yaWdpbmFsTWF0ZXJpYWw6IFRIUkVFLk1hdGVyaWFsLFxuICAgIHZybVByb3BzOiBWUk1TY2hlbWEuTWF0ZXJpYWwsXG4gICAgZ2x0ZjogR0xURixcbiAgKTogUHJvbWlzZTxhbnk+IHtcbiAgICBjb25zdCB0YXNrTGlzdDogQXJyYXk8UHJvbWlzZTxhbnk+PiA9IFtdO1xuICAgIGNvbnN0IHBhcmFtczogYW55ID0ge307XG5cbiAgICAvLyBleHRyYWN0IHRleHR1cmUgcHJvcGVydGllc1xuICAgIGlmICh2cm1Qcm9wcy50ZXh0dXJlUHJvcGVydGllcykge1xuICAgICAgZm9yIChjb25zdCBuYW1lIG9mIE9iamVjdC5rZXlzKHZybVByb3BzLnRleHR1cmVQcm9wZXJ0aWVzKSkge1xuICAgICAgICBjb25zdCBuZXdOYW1lID0gdGhpcy5fcmVuYW1lTWF0ZXJpYWxQcm9wZXJ0eShuYW1lKTtcbiAgICAgICAgY29uc3QgdGV4dHVyZUluZGV4ID0gdnJtUHJvcHMudGV4dHVyZVByb3BlcnRpZXNbbmFtZV07XG5cbiAgICAgICAgdGFza0xpc3QucHVzaChcbiAgICAgICAgICBnbHRmLnBhcnNlci5nZXREZXBlbmRlbmN5KCd0ZXh0dXJlJywgdGV4dHVyZUluZGV4KS50aGVuKCh0ZXh0dXJlOiBUSFJFRS5UZXh0dXJlKSA9PiB7XG4gICAgICAgICAgICBwYXJhbXNbbmV3TmFtZV0gPSB0ZXh0dXJlO1xuICAgICAgICAgIH0pLFxuICAgICAgICApO1xuICAgICAgfVxuICAgIH1cblxuICAgIC8vIGV4dHJhY3QgZmxvYXQgcHJvcGVydGllc1xuICAgIGlmICh2cm1Qcm9wcy5mbG9hdFByb3BlcnRpZXMpIHtcbiAgICAgIGZvciAoY29uc3QgbmFtZSBvZiBPYmplY3Qua2V5cyh2cm1Qcm9wcy5mbG9hdFByb3BlcnRpZXMpKSB7XG4gICAgICAgIGNvbnN0IG5ld05hbWUgPSB0aGlzLl9yZW5hbWVNYXRlcmlhbFByb3BlcnR5KG5hbWUpO1xuICAgICAgICBwYXJhbXNbbmV3TmFtZV0gPSB2cm1Qcm9wcy5mbG9hdFByb3BlcnRpZXNbbmFtZV07XG4gICAgICB9XG4gICAgfVxuXG4gICAgLy8gZXh0cmFjdCB2ZWN0b3IgKGNvbG9yIHRiaCkgcHJvcGVydGllc1xuICAgIGlmICh2cm1Qcm9wcy52ZWN0b3JQcm9wZXJ0aWVzKSB7XG4gICAgICBmb3IgKGNvbnN0IG5hbWUgb2YgT2JqZWN0LmtleXModnJtUHJvcHMudmVjdG9yUHJvcGVydGllcykpIHtcbiAgICAgICAgbGV0IG5ld05hbWUgPSB0aGlzLl9yZW5hbWVNYXRlcmlhbFByb3BlcnR5KG5hbWUpO1xuXG4gICAgICAgIC8vIGlmIHRoaXMgaXMgdGV4dHVyZVNUIChzYW1lIG5hbWUgYXMgdGV4dHVyZSBuYW1lIGl0c2VsZiksIGFkZCAnX1NUJ1xuICAgICAgICBjb25zdCBpc1RleHR1cmVTVCA9IFtcbiAgICAgICAgICAnX01haW5UZXgnLFxuICAgICAgICAgICdfU2hhZGVUZXh0dXJlJyxcbiAgICAgICAgICAnX0J1bXBNYXAnLFxuICAgICAgICAgICdfUmVjZWl2ZVNoYWRvd1RleHR1cmUnLFxuICAgICAgICAgICdfU2hhZGluZ0dyYWRlVGV4dHVyZScsXG4gICAgICAgICAgJ19SaW1UZXh0dXJlJyxcbiAgICAgICAgICAnX1NwaGVyZUFkZCcsXG4gICAgICAgICAgJ19FbWlzc2lvbk1hcCcsXG4gICAgICAgICAgJ19PdXRsaW5lV2lkdGhUZXh0dXJlJyxcbiAgICAgICAgICAnX1V2QW5pbU1hc2tUZXh0dXJlJyxcbiAgICAgICAgXS5zb21lKCh0ZXh0dXJlTmFtZSkgPT4gbmFtZSA9PT0gdGV4dHVyZU5hbWUpO1xuICAgICAgICBpZiAoaXNUZXh0dXJlU1QpIHtcbiAgICAgICAgICBuZXdOYW1lICs9ICdfU1QnO1xuICAgICAgICB9XG5cbiAgICAgICAgcGFyYW1zW25ld05hbWVdID0gbmV3IFRIUkVFLlZlY3RvcjQoLi4udnJtUHJvcHMudmVjdG9yUHJvcGVydGllc1tuYW1lXSk7XG4gICAgICB9XG4gICAgfVxuXG4gICAgLy8gc2V0IHdoZXRoZXIgaXQgbmVlZHMgc2tpbm5pbmcgYW5kIG1vcnBoaW5nIG9yIG5vdFxuICAgIHBhcmFtcy5za2lubmluZyA9IChvcmlnaW5hbE1hdGVyaWFsIGFzIGFueSkuc2tpbm5pbmcgfHwgZmFsc2U7XG4gICAgcGFyYW1zLm1vcnBoVGFyZ2V0cyA9IChvcmlnaW5hbE1hdGVyaWFsIGFzIGFueSkubW9ycGhUYXJnZXRzIHx8IGZhbHNlO1xuICAgIHBhcmFtcy5tb3JwaE5vcm1hbHMgPSAob3JpZ2luYWxNYXRlcmlhbCBhcyBhbnkpLm1vcnBoTm9ybWFscyB8fCBmYWxzZTtcblxuICAgIHJldHVybiBQcm9taXNlLmFsbCh0YXNrTGlzdCkudGhlbigoKSA9PiBwYXJhbXMpO1xuICB9XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5pbXBvcnQgeyBWUk1TY2hlbWEgfSBmcm9tICcuLi90eXBlcyc7XG5pbXBvcnQgeyBHTFRGIH0gZnJvbSAndGhyZWUvZXhhbXBsZXMvanNtL2xvYWRlcnMvR0xURkxvYWRlcic7XG5pbXBvcnQgeyBWUk1NZXRhIH0gZnJvbSAnLi9WUk1NZXRhJztcbmltcG9ydCB7IFZSTU1ldGFJbXBvcnRlck9wdGlvbnMgfSBmcm9tICcuL1ZSTU1ldGFJbXBvcnRlck9wdGlvbnMnO1xuXG4vKipcbiAqIEFuIGltcG9ydGVyIHRoYXQgaW1wb3J0cyBhIHtAbGluayBWUk1NZXRhfSBmcm9tIGEgVlJNIGV4dGVuc2lvbiBvZiBhIEdMVEYuXG4gKi9cbmV4cG9ydCBjbGFzcyBWUk1NZXRhSW1wb3J0ZXIge1xuICAvKipcbiAgICogSWYgYHRydWVgLCBpdCB3b24ndCBsb2FkIGl0cyB0aHVtYm5haWwgdGV4dHVyZSAoe0BsaW5rIFZSTU1ldGEudGV4dHVyZX0pLiBgZmFsc2VgIGJ5IGRlZmF1bHQuXG4gICAqL1xuICBwdWJsaWMgaWdub3JlVGV4dHVyZTogYm9vbGVhbjtcblxuICBjb25zdHJ1Y3RvcihvcHRpb25zPzogVlJNTWV0YUltcG9ydGVyT3B0aW9ucykge1xuICAgIHRoaXMuaWdub3JlVGV4dHVyZSA9IG9wdGlvbnM/Lmlnbm9yZVRleHR1cmUgPz8gZmFsc2U7XG4gIH1cblxuICBwdWJsaWMgYXN5bmMgaW1wb3J0KGdsdGY6IEdMVEYpOiBQcm9taXNlPFZSTU1ldGEgfCBudWxsPiB7XG4gICAgY29uc3QgdnJtRXh0OiBWUk1TY2hlbWEuVlJNIHwgdW5kZWZpbmVkID0gZ2x0Zi5wYXJzZXIuanNvbi5leHRlbnNpb25zPy5WUk07XG4gICAgaWYgKCF2cm1FeHQpIHtcbiAgICAgIHJldHVybiBudWxsO1xuICAgIH1cblxuICAgIGNvbnN0IHNjaGVtYU1ldGE6IFZSTVNjaGVtYS5NZXRhIHwgdW5kZWZpbmVkID0gdnJtRXh0Lm1ldGE7XG4gICAgaWYgKCFzY2hlbWFNZXRhKSB7XG4gICAgICByZXR1cm4gbnVsbDtcbiAgICB9XG5cbiAgICBsZXQgdGV4dHVyZTogVEhSRUUuVGV4dHVyZSB8IG51bGwgfCB1bmRlZmluZWQ7XG4gICAgaWYgKCF0aGlzLmlnbm9yZVRleHR1cmUgJiYgc2NoZW1hTWV0YS50ZXh0dXJlICE9IG51bGwgJiYgc2NoZW1hTWV0YS50ZXh0dXJlICE9PSAtMSkge1xuICAgICAgdGV4dHVyZSA9IGF3YWl0IGdsdGYucGFyc2VyLmdldERlcGVuZGVuY3koJ3RleHR1cmUnLCBzY2hlbWFNZXRhLnRleHR1cmUpO1xuICAgIH1cblxuICAgIHJldHVybiB7XG4gICAgICBhbGxvd2VkVXNlck5hbWU6IHNjaGVtYU1ldGEuYWxsb3dlZFVzZXJOYW1lLFxuICAgICAgYXV0aG9yOiBzY2hlbWFNZXRhLmF1dGhvcixcbiAgICAgIGNvbW1lcmNpYWxVc3NhZ2VOYW1lOiBzY2hlbWFNZXRhLmNvbW1lcmNpYWxVc3NhZ2VOYW1lLFxuICAgICAgY29udGFjdEluZm9ybWF0aW9uOiBzY2hlbWFNZXRhLmNvbnRhY3RJbmZvcm1hdGlvbixcbiAgICAgIGxpY2Vuc2VOYW1lOiBzY2hlbWFNZXRhLmxpY2Vuc2VOYW1lLFxuICAgICAgb3RoZXJMaWNlbnNlVXJsOiBzY2hlbWFNZXRhLm90aGVyTGljZW5zZVVybCxcbiAgICAgIG90aGVyUGVybWlzc2lvblVybDogc2NoZW1hTWV0YS5vdGhlclBlcm1pc3Npb25VcmwsXG4gICAgICByZWZlcmVuY2U6IHNjaGVtYU1ldGEucmVmZXJlbmNlLFxuICAgICAgc2V4dWFsVXNzYWdlTmFtZTogc2NoZW1hTWV0YS5zZXh1YWxVc3NhZ2VOYW1lLFxuICAgICAgdGV4dHVyZTogdGV4dHVyZSA/PyB1bmRlZmluZWQsXG4gICAgICB0aXRsZTogc2NoZW1hTWV0YS50aXRsZSxcbiAgICAgIHZlcnNpb246IHNjaGVtYU1ldGEudmVyc2lvbixcbiAgICAgIHZpb2xlbnRVc3NhZ2VOYW1lOiBzY2hlbWFNZXRhLnZpb2xlbnRVc3NhZ2VOYW1lLFxuICAgIH07XG4gIH1cbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcblxuY29uc3QgX21hdEEgPSBuZXcgVEhSRUUuTWF0cml4NCgpO1xuXG4vKipcbiAqIEEgY29tcGF0IGZ1bmN0aW9uIGZvciBgTWF0cml4NC5pbnZlcnQoKWAgLyBgTWF0cml4NC5nZXRJbnZlcnNlKClgLlxuICogYE1hdHJpeDQuaW52ZXJ0KClgIGlzIGludHJvZHVjZWQgaW4gcjEyMyBhbmQgYE1hdHJpeDQuZ2V0SW52ZXJzZSgpYCBlbWl0cyBhIHdhcm5pbmcuXG4gKiBXZSBhcmUgZ29pbmcgdG8gdXNlIHRoaXMgY29tcGF0IGZvciBhIHdoaWxlLlxuICogQHBhcmFtIHRhcmdldCBBIHRhcmdldCBtYXRyaXhcbiAqL1xuZXhwb3J0IGZ1bmN0aW9uIG1hdDRJbnZlcnRDb21wYXQ8VCBleHRlbmRzIFRIUkVFLk1hdHJpeDQ+KHRhcmdldDogVCk6IFQge1xuICBpZiAoKHRhcmdldCBhcyBhbnkpLmludmVydCkge1xuICAgIHRhcmdldC5pbnZlcnQoKTtcbiAgfSBlbHNlIHtcbiAgICAodGFyZ2V0IGFzIGFueSkuZ2V0SW52ZXJzZShfbWF0QS5jb3B5KHRhcmdldCkpO1xuICB9XG5cbiAgcmV0dXJuIHRhcmdldDtcbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcbmltcG9ydCB7IG1hdDRJbnZlcnRDb21wYXQgfSBmcm9tICcuL21hdDRJbnZlcnRDb21wYXQnO1xuXG5leHBvcnQgY2xhc3MgTWF0cml4NEludmVyc2VDYWNoZSB7XG4gIC8qKlxuICAgKiBUaGUgdGFyZ2V0IG1hdHJpeC5cbiAgICovXG4gIHB1YmxpYyByZWFkb25seSBtYXRyaXg6IFRIUkVFLk1hdHJpeDQ7XG5cbiAgLyoqXG4gICAqIEEgY2FjaGUgb2YgaW52ZXJzZSBvZiBjdXJyZW50IG1hdHJpeC5cbiAgICovXG4gIHByaXZhdGUgcmVhZG9ubHkgX2ludmVyc2VDYWNoZSA9IG5ldyBUSFJFRS5NYXRyaXg0KCk7XG5cbiAgLyoqXG4gICAqIEEgZmxhZyB0aGF0IG1ha2VzIGl0IHdhbnQgdG8gcmVjYWxjdWxhdGUgaXRzIHtAbGluayBfaW52ZXJzZUNhY2hlfS5cbiAgICogV2lsbCBiZSBzZXQgYHRydWVgIHdoZW4gYGVsZW1lbnRzYCBhcmUgbXV0YXRlZCBhbmQgYmUgdXNlZCBpbiBgZ2V0SW52ZXJzZWAuXG4gICAqL1xuICBwcml2YXRlIF9zaG91bGRVcGRhdGVJbnZlcnNlID0gdHJ1ZTtcblxuICAvKipcbiAgICogVGhlIG9yaWdpbmFsIG9mIGBtYXRyaXguZWxlbWVudHNgXG4gICAqL1xuICBwcml2YXRlIHJlYWRvbmx5IF9vcmlnaW5hbEVsZW1lbnRzOiBudW1iZXJbXTtcblxuICAvKipcbiAgICogSW52ZXJzZSBvZiBnaXZlbiBtYXRyaXguXG4gICAqIE5vdGUgdGhhdCBpdCB3aWxsIHJldHVybiBpdHMgaW50ZXJuYWwgcHJpdmF0ZSBpbnN0YW5jZS5cbiAgICogTWFrZSBzdXJlIGNvcHlpbmcgdGhpcyBiZWZvcmUgbXV0YXRlIHRoaXMuXG4gICAqL1xuICBwdWJsaWMgZ2V0IGludmVyc2UoKTogVEhSRUUuTWF0cml4NCB7XG4gICAgaWYgKHRoaXMuX3Nob3VsZFVwZGF0ZUludmVyc2UpIHtcbiAgICAgIG1hdDRJbnZlcnRDb21wYXQodGhpcy5faW52ZXJzZUNhY2hlLmNvcHkodGhpcy5tYXRyaXgpKTtcbiAgICAgIHRoaXMuX3Nob3VsZFVwZGF0ZUludmVyc2UgPSBmYWxzZTtcbiAgICB9XG5cbiAgICByZXR1cm4gdGhpcy5faW52ZXJzZUNhY2hlO1xuICB9XG5cbiAgcHVibGljIGNvbnN0cnVjdG9yKG1hdHJpeDogVEhSRUUuTWF0cml4NCkge1xuICAgIHRoaXMubWF0cml4ID0gbWF0cml4O1xuXG4gICAgY29uc3QgaGFuZGxlcjogUHJveHlIYW5kbGVyPG51bWJlcltdPiA9IHtcbiAgICAgIHNldDogKG9iaiwgcHJvcDogbnVtYmVyLCBuZXdWYWwpID0+IHtcbiAgICAgICAgdGhpcy5fc2hvdWxkVXBkYXRlSW52ZXJzZSA9IHRydWU7XG4gICAgICAgIG9ialtwcm9wXSA9IG5ld1ZhbDtcblxuICAgICAgICByZXR1cm4gdHJ1ZTtcbiAgICAgIH0sXG4gICAgfTtcblxuICAgIHRoaXMuX29yaWdpbmFsRWxlbWVudHMgPSBtYXRyaXguZWxlbWVudHM7XG4gICAgbWF0cml4LmVsZW1lbnRzID0gbmV3IFByb3h5KG1hdHJpeC5lbGVtZW50cywgaGFuZGxlcik7XG4gIH1cblxuICBwdWJsaWMgcmV2ZXJ0KCk6IHZvaWQge1xuICAgIHRoaXMubWF0cml4LmVsZW1lbnRzID0gdGhpcy5fb3JpZ2luYWxFbGVtZW50cztcbiAgfVxufVxuIiwiaW1wb3J0ICogYXMgVEhSRUUgZnJvbSAndGhyZWUnO1xuaW1wb3J0IHsgbWF0NEludmVydENvbXBhdCB9IGZyb20gJy4uL3V0aWxzL21hdDRJbnZlcnRDb21wYXQnO1xuaW1wb3J0IHsgZ2V0V29ybGRRdWF0ZXJuaW9uTGl0ZSB9IGZyb20gJy4uL3V0aWxzL21hdGgnO1xuaW1wb3J0IHsgTWF0cml4NEludmVyc2VDYWNoZSB9IGZyb20gJy4uL3V0aWxzL01hdHJpeDRJbnZlcnNlQ2FjaGUnO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZUNvbGxpZGVyTWVzaCB9IGZyb20gJy4vVlJNU3ByaW5nQm9uZUNvbGxpZGVyR3JvdXAnO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZVBhcmFtZXRlcnMgfSBmcm9tICcuL1ZSTVNwcmluZ0JvbmVQYXJhbWV0ZXJzJztcbi8vIGJhc2VkIG9uXG4vLyBodHRwOi8vcm9ja2V0anVtcC5za3IuanAvdW5pdHkzZC8xMDkvXG4vLyBodHRwczovL2dpdGh1Yi5jb20vZHdhbmdvL1VuaVZSTS9ibG9iL21hc3Rlci9TY3JpcHRzL1NwcmluZ0JvbmUvVlJNU3ByaW5nQm9uZS5jc1xuXG5jb25zdCBJREVOVElUWV9NQVRSSVg0ID0gT2JqZWN0LmZyZWV6ZShuZXcgVEhSRUUuTWF0cml4NCgpKTtcbmNvbnN0IElERU5USVRZX1FVQVRFUk5JT04gPSBPYmplY3QuZnJlZXplKG5ldyBUSFJFRS5RdWF0ZXJuaW9uKCkpO1xuXG4vLyDoqIjnrpfkuK3jga7kuIDmmYLkv53lrZjnlKjlpInmlbDvvIjkuIDluqbjgqTjg7Pjgrnjgr/jg7PjgrnjgpLkvZzjgaPjgZ/jgonjgYLjgajjga/kvb/jgYTlm57jgZnvvIlcbmNvbnN0IF92M0EgPSBuZXcgVEhSRUUuVmVjdG9yMygpO1xuY29uc3QgX3YzQiA9IG5ldyBUSFJFRS5WZWN0b3IzKCk7XG5jb25zdCBfdjNDID0gbmV3IFRIUkVFLlZlY3RvcjMoKTtcbmNvbnN0IF9xdWF0QSA9IG5ldyBUSFJFRS5RdWF0ZXJuaW9uKCk7XG5jb25zdCBfbWF0QSA9IG5ldyBUSFJFRS5NYXRyaXg0KCk7XG5jb25zdCBfbWF0QiA9IG5ldyBUSFJFRS5NYXRyaXg0KCk7XG5cbi8qKlxuICogQSBjbGFzcyByZXByZXNlbnRzIGEgc2luZ2xlIHNwcmluZyBib25lIG9mIGEgVlJNLlxuICogSXQgc2hvdWxkIGJlIG1hbmFnZWQgYnkgYSBbW1ZSTVNwcmluZ0JvbmVNYW5hZ2VyXV0uXG4gKi9cbmV4cG9ydCBjbGFzcyBWUk1TcHJpbmdCb25lIHtcbiAgLyoqXG4gICAqIFJhZGl1cyBvZiB0aGUgYm9uZSwgd2lsbCBiZSB1c2VkIGZvciBjb2xsaXNpb24uXG4gICAqL1xuICBwdWJsaWMgcmFkaXVzOiBudW1iZXI7XG5cbiAgLyoqXG4gICAqIFN0aWZmbmVzcyBmb3JjZSBvZiB0aGUgYm9uZS4gSW5jcmVhc2luZyB0aGUgdmFsdWUgPSBmYXN0ZXIgY29udmVyZ2VuY2UgKGZlZWxzIFwiaGFyZGVyXCIpLlxuICAgKiBPbiBVbmlWUk0sIGl0cyByYW5nZSBvbiBHVUkgaXMgYmV0d2VlbiBgMC4wYCBhbmQgYDQuMGAgLlxuICAgKi9cbiAgcHVibGljIHN0aWZmbmVzc0ZvcmNlOiBudW1iZXI7XG5cbiAgLyoqXG4gICAqIFBvd2VyIG9mIHRoZSBncmF2aXR5IGFnYWluc3QgdGhpcyBib25lLlxuICAgKiBUaGUgXCJwb3dlclwiIHVzZWQgaW4gaGVyZSBpcyB2ZXJ5IGZhciBmcm9tIHNjaWVudGlmaWMgcGh5c2ljcyB0ZXJtLi4uXG4gICAqL1xuICBwdWJsaWMgZ3Jhdml0eVBvd2VyOiBudW1iZXI7XG5cbiAgLyoqXG4gICAqIERpcmVjdGlvbiBvZiB0aGUgZ3Jhdml0eSBhZ2FpbnN0IHRoaXMgYm9uZS5cbiAgICogVXN1YWxseSBpdCBzaG91bGQgYmUgbm9ybWFsaXplZC5cbiAgICovXG4gIHB1YmxpYyBncmF2aXR5RGlyOiBUSFJFRS5WZWN0b3IzO1xuXG4gIC8qKlxuICAgKiBEcmFnIGZvcmNlIG9mIHRoZSBib25lLiBJbmNyZWFzaW5nIHRoZSB2YWx1ZSA9IGxlc3Mgb3NjaWxsYXRpb24gKGZlZWxzIFwiaGVhdmllclwiKS5cbiAgICogT24gVW5pVlJNLCBpdHMgcmFuZ2Ugb24gR1VJIGlzIGJldHdlZW4gYDAuMGAgYW5kIGAxLjBgIC5cbiAgICovXG4gIHB1YmxpYyBkcmFnRm9yY2U6IG51bWJlcjtcblxuICAvKipcbiAgICogQ29sbGlkZXIgZ3JvdXBzIGF0dGFjaGVkIHRvIHRoaXMgYm9uZS5cbiAgICovXG4gIHB1YmxpYyBjb2xsaWRlcnM6IFZSTVNwcmluZ0JvbmVDb2xsaWRlck1lc2hbXTtcblxuICAvKipcbiAgICogQW4gT2JqZWN0M0QgYXR0YWNoZWQgdG8gdGhpcyBib25lLlxuICAgKi9cbiAgcHVibGljIHJlYWRvbmx5IGJvbmU6IFRIUkVFLk9iamVjdDNEO1xuXG4gIC8qKlxuICAgKiBDdXJyZW50IHBvc2l0aW9uIG9mIGNoaWxkIHRhaWwsIGluIHdvcmxkIHVuaXQuIFdpbGwgYmUgdXNlZCBmb3IgdmVybGV0IGludGVncmF0aW9uLlxuICAgKi9cbiAgcHJvdGVjdGVkIF9jdXJyZW50VGFpbCA9IG5ldyBUSFJFRS5WZWN0b3IzKCk7XG5cbiAgLyoqXG4gICAqIFByZXZpb3VzIHBvc2l0aW9uIG9mIGNoaWxkIHRhaWwsIGluIHdvcmxkIHVuaXQuIFdpbGwgYmUgdXNlZCBmb3IgdmVybGV0IGludGVncmF0aW9uLlxuICAgKi9cbiAgcHJvdGVjdGVkIF9wcmV2VGFpbCA9IG5ldyBUSFJFRS5WZWN0b3IzKCk7XG5cbiAgLyoqXG4gICAqIE5leHQgcG9zaXRpb24gb2YgY2hpbGQgdGFpbCwgaW4gd29ybGQgdW5pdC4gV2lsbCBiZSB1c2VkIGZvciB2ZXJsZXQgaW50ZWdyYXRpb24uXG4gICAqIEFjdHVhbGx5IHVzZWQgb25seSBpbiBbW3VwZGF0ZV1dIGFuZCBpdCdzIGtpbmQgb2YgdGVtcG9yYXJ5IHZhcmlhYmxlLlxuICAgKi9cbiAgcHJvdGVjdGVkIF9uZXh0VGFpbCA9IG5ldyBUSFJFRS5WZWN0b3IzKCk7XG5cbiAgLyoqXG4gICAqIEluaXRpYWwgYXhpcyBvZiB0aGUgYm9uZSwgaW4gbG9jYWwgdW5pdC5cbiAgICovXG4gIHByb3RlY3RlZCBfYm9uZUF4aXMgPSBuZXcgVEhSRUUuVmVjdG9yMygpO1xuXG4gIC8qKlxuICAgKiBMZW5ndGggb2YgdGhlIGJvbmUgaW4gcmVsYXRpdmUgc3BhY2UgdW5pdC4gV2lsbCBiZSB1c2VkIGZvciBub3JtYWxpemF0aW9uIGluIHVwZGF0ZSBsb29wLlxuICAgKiBJdCdzIHNhbWUgYXMgbG9jYWwgdW5pdCBsZW5ndGggdW5sZXNzIHRoZXJlIGFyZSBzY2FsZSB0cmFuc2Zvcm1hdGlvbiBpbiB3b3JsZCBtYXRyaXguXG4gICAqL1xuICBwcm90ZWN0ZWQgX2NlbnRlclNwYWNlQm9uZUxlbmd0aDogbnVtYmVyO1xuXG4gIC8qKlxuICAgKiBQb3NpdGlvbiBvZiB0aGlzIGJvbmUgaW4gcmVsYXRpdmUgc3BhY2UsIGtpbmQgb2YgYSB0ZW1wb3JhcnkgdmFyaWFibGUuXG4gICAqL1xuICBwcm90ZWN0ZWQgX2NlbnRlclNwYWNlUG9zaXRpb24gPSBuZXcgVEhSRUUuVmVjdG9yMygpO1xuXG4gIC8qKlxuICAgKiBUaGlzIHNwcmluZ2JvbmUgd2lsbCBiZSBjYWxjdWxhdGVkIGJhc2VkIG9uIHRoZSBzcGFjZSByZWxhdGl2ZSBmcm9tIHRoaXMgb2JqZWN0LlxuICAgKiBJZiB0aGlzIGlzIGBudWxsYCwgc3ByaW5nYm9uZSB3aWxsIGJlIGNhbGN1bGF0ZWQgaW4gd29ybGQgc3BhY2UuXG4gICAqL1xuICBwcm90ZWN0ZWQgX2NlbnRlcjogVEhSRUUuT2JqZWN0M0QgfCBudWxsID0gbnVsbDtcbiAgcHVibGljIGdldCBjZW50ZXIoKTogVEhSRUUuT2JqZWN0M0QgfCBudWxsIHtcbiAgICByZXR1cm4gdGhpcy5fY2VudGVyO1xuICB9XG4gIHB1YmxpYyBzZXQgY2VudGVyKGNlbnRlcjogVEhSRUUuT2JqZWN0M0QgfCBudWxsKSB7XG4gICAgLy8gY29udmVydCB0YWlscyB0byB3b3JsZCBzcGFjZVxuICAgIHRoaXMuX2dldE1hdHJpeENlbnRlclRvV29ybGQoX21hdEEpO1xuXG4gICAgdGhpcy5fY3VycmVudFRhaWwuYXBwbHlNYXRyaXg0KF9tYXRBKTtcbiAgICB0aGlzLl9wcmV2VGFpbC5hcHBseU1hdHJpeDQoX21hdEEpO1xuICAgIHRoaXMuX25leHRUYWlsLmFwcGx5TWF0cml4NChfbWF0QSk7XG5cbiAgICAvLyB1bmluc3RhbGwgaW52ZXJzZSBjYWNoZVxuICAgIGlmICh0aGlzLl9jZW50ZXI/LnVzZXJEYXRhLmludmVyc2VDYWNoZVByb3h5KSB7XG4gICAgICAodGhpcy5fY2VudGVyLnVzZXJEYXRhLmludmVyc2VDYWNoZVByb3h5IGFzIE1hdHJpeDRJbnZlcnNlQ2FjaGUpLnJldmVydCgpO1xuICAgICAgZGVsZXRlIHRoaXMuX2NlbnRlci51c2VyRGF0YS5pbnZlcnNlQ2FjaGVQcm94eTtcbiAgICB9XG5cbiAgICAvLyBjaGFuZ2UgdGhlIGNlbnRlclxuICAgIHRoaXMuX2NlbnRlciA9IGNlbnRlcjtcblxuICAgIC8vIGluc3RhbGwgaW52ZXJzZSBjYWNoZVxuICAgIGlmICh0aGlzLl9jZW50ZXIpIHtcbiAgICAgIGlmICghdGhpcy5fY2VudGVyLnVzZXJEYXRhLmludmVyc2VDYWNoZVByb3h5KSB7XG4gICAgICAgIHRoaXMuX2NlbnRlci51c2VyRGF0YS5pbnZlcnNlQ2FjaGVQcm94eSA9IG5ldyBNYXRyaXg0SW52ZXJzZUNhY2hlKHRoaXMuX2NlbnRlci5tYXRyaXhXb3JsZCk7XG4gICAgICB9XG4gICAgfVxuXG4gICAgLy8gY29udmVydCB0YWlscyB0byBjZW50ZXIgc3BhY2VcbiAgICB0aGlzLl9nZXRNYXRyaXhXb3JsZFRvQ2VudGVyKF9tYXRBKTtcblxuICAgIHRoaXMuX2N1cnJlbnRUYWlsLmFwcGx5TWF0cml4NChfbWF0QSk7XG4gICAgdGhpcy5fcHJldlRhaWwuYXBwbHlNYXRyaXg0KF9tYXRBKTtcbiAgICB0aGlzLl9uZXh0VGFpbC5hcHBseU1hdHJpeDQoX21hdEEpO1xuXG4gICAgLy8gY29udmVydCBjZW50ZXIgc3BhY2UgZGVwZW5kYW50IHN0YXRlXG4gICAgX21hdEEubXVsdGlwbHkodGhpcy5ib25lLm1hdHJpeFdvcmxkKTsgLy8g8J+UpSA/P1xuXG4gICAgdGhpcy5fY2VudGVyU3BhY2VQb3NpdGlvbi5zZXRGcm9tTWF0cml4UG9zaXRpb24oX21hdEEpO1xuXG4gICAgdGhpcy5fY2VudGVyU3BhY2VCb25lTGVuZ3RoID0gX3YzQVxuICAgICAgLmNvcHkodGhpcy5faW5pdGlhbExvY2FsQ2hpbGRQb3NpdGlvbilcbiAgICAgIC5hcHBseU1hdHJpeDQoX21hdEEpXG4gICAgICAuc3ViKHRoaXMuX2NlbnRlclNwYWNlUG9zaXRpb24pXG4gICAgICAubGVuZ3RoKCk7XG4gIH1cblxuICAvKipcbiAgICogUm90YXRpb24gb2YgcGFyZW50IGJvbmUsIGluIHdvcmxkIHVuaXQuXG4gICAqIFdlIHNob3VsZCB1cGRhdGUgdGhpcyBjb25zdGFudGx5IGluIFtbdXBkYXRlXV0uXG4gICAqL1xuICBwcml2YXRlIF9wYXJlbnRXb3JsZFJvdGF0aW9uID0gbmV3IFRIUkVFLlF1YXRlcm5pb24oKTtcblxuICAvKipcbiAgICogSW5pdGlhbCBzdGF0ZSBvZiB0aGUgbG9jYWwgbWF0cml4IG9mIHRoZSBib25lLlxuICAgKi9cbiAgcHJpdmF0ZSBfaW5pdGlhbExvY2FsTWF0cml4ID0gbmV3IFRIUkVFLk1hdHJpeDQoKTtcblxuICAvKipcbiAgICogSW5pdGlhbCBzdGF0ZSBvZiB0aGUgcm90YXRpb24gb2YgdGhlIGJvbmUuXG4gICAqL1xuICBwcml2YXRlIF9pbml0aWFsTG9jYWxSb3RhdGlvbiA9IG5ldyBUSFJFRS5RdWF0ZXJuaW9uKCk7XG5cbiAgLyoqXG4gICAqIEluaXRpYWwgc3RhdGUgb2YgdGhlIHBvc2l0aW9uIG9mIGl0cyBjaGlsZC5cbiAgICovXG4gIHByaXZhdGUgX2luaXRpYWxMb2NhbENoaWxkUG9zaXRpb24gPSBuZXcgVEhSRUUuVmVjdG9yMygpO1xuXG4gIC8qKlxuICAgKiBDcmVhdGUgYSBuZXcgVlJNU3ByaW5nQm9uZS5cbiAgICpcbiAgICogQHBhcmFtIGJvbmUgQW4gT2JqZWN0M0QgdGhhdCB3aWxsIGJlIGF0dGFjaGVkIHRvIHRoaXMgYm9uZVxuICAgKiBAcGFyYW0gcGFyYW1zIFNldmVyYWwgcGFyYW1ldGVycyByZWxhdGVkIHRvIGJlaGF2aW9yIG9mIHRoZSBzcHJpbmcgYm9uZVxuICAgKi9cbiAgY29uc3RydWN0b3IoYm9uZTogVEhSRUUuT2JqZWN0M0QsIHBhcmFtczogVlJNU3ByaW5nQm9uZVBhcmFtZXRlcnMgPSB7fSkge1xuICAgIHRoaXMuYm9uZSA9IGJvbmU7IC8vIHVuaVZSTeOBp+OBriBwYXJlbnRcbiAgICB0aGlzLmJvbmUubWF0cml4QXV0b1VwZGF0ZSA9IGZhbHNlOyAvLyB1cGRhdGXjgavjgojjgoroqIjnrpfjgZXjgozjgovjga7jgad0aHJlZS5qc+WGheOBp+OBruiHquWLleWHpueQhuOBr+S4jeimgVxuXG4gICAgdGhpcy5yYWRpdXMgPSBwYXJhbXMucmFkaXVzID8/IDAuMDI7XG4gICAgdGhpcy5zdGlmZm5lc3NGb3JjZSA9IHBhcmFtcy5zdGlmZm5lc3NGb3JjZSA/PyAxLjA7XG4gICAgdGhpcy5ncmF2aXR5RGlyID0gcGFyYW1zLmdyYXZpdHlEaXJcbiAgICAgID8gbmV3IFRIUkVFLlZlY3RvcjMoKS5jb3B5KHBhcmFtcy5ncmF2aXR5RGlyKVxuICAgICAgOiBuZXcgVEhSRUUuVmVjdG9yMygpLnNldCgwLjAsIC0xLjAsIDAuMCk7XG4gICAgdGhpcy5ncmF2aXR5UG93ZXIgPSBwYXJhbXMuZ3Jhdml0eVBvd2VyID8/IDAuMDtcbiAgICB0aGlzLmRyYWdGb3JjZSA9IHBhcmFtcy5kcmFnRm9yY2UgPz8gMC40O1xuICAgIHRoaXMuY29sbGlkZXJzID0gcGFyYW1zLmNvbGxpZGVycyA/PyBbXTtcblxuICAgIHRoaXMuX2NlbnRlclNwYWNlUG9zaXRpb24uc2V0RnJvbU1hdHJpeFBvc2l0aW9uKHRoaXMuYm9uZS5tYXRyaXhXb3JsZCk7XG5cbiAgICB0aGlzLl9pbml0aWFsTG9jYWxNYXRyaXguY29weSh0aGlzLmJvbmUubWF0cml4KTtcbiAgICB0aGlzLl9pbml0aWFsTG9jYWxSb3RhdGlvbi5jb3B5KHRoaXMuYm9uZS5xdWF0ZXJuaW9uKTtcblxuICAgIGlmICh0aGlzLmJvbmUuY2hpbGRyZW4ubGVuZ3RoID09PSAwKSB7XG4gICAgICAvLyDmnKvnq6/jga7jg5zjg7zjg7PjgILlrZDjg5zjg7zjg7PjgYzjgYTjgarjgYTjgZ/jgoHjgIzoh6rliIbjga7lsJHjgZflhYjjgI3jgYzlrZDjg5zjg7zjg7PjgajjgYTjgYbjgZPjgajjgavjgZnjgotcbiAgICAgIC8vIGh0dHBzOi8vZ2l0aHViLmNvbS9kd2FuZ28vVW5pVlJNL2Jsb2IvbWFzdGVyL0Fzc2V0cy9WUk0vVW5pVlJNL1NjcmlwdHMvU3ByaW5nQm9uZS9WUk1TcHJpbmdCb25lLmNzI0wyNDZcbiAgICAgIHRoaXMuX2luaXRpYWxMb2NhbENoaWxkUG9zaXRpb24uY29weSh0aGlzLmJvbmUucG9zaXRpb24pLm5vcm1hbGl6ZSgpLm11bHRpcGx5U2NhbGFyKDAuMDcpOyAvLyBtYWdpYyBudW1iZXIhIGRlcml2ZXMgZnJvbSBvcmlnaW5hbCBzb3VyY2VcbiAgICB9IGVsc2Uge1xuICAgICAgY29uc3QgZmlyc3RDaGlsZCA9IHRoaXMuYm9uZS5jaGlsZHJlblswXTtcbiAgICAgIHRoaXMuX2luaXRpYWxMb2NhbENoaWxkUG9zaXRpb24uY29weShmaXJzdENoaWxkLnBvc2l0aW9uKTtcbiAgICB9XG5cbiAgICB0aGlzLmJvbmUubG9jYWxUb1dvcmxkKHRoaXMuX2N1cnJlbnRUYWlsLmNvcHkodGhpcy5faW5pdGlhbExvY2FsQ2hpbGRQb3NpdGlvbikpO1xuICAgIHRoaXMuX3ByZXZUYWlsLmNvcHkodGhpcy5fY3VycmVudFRhaWwpO1xuICAgIHRoaXMuX25leHRUYWlsLmNvcHkodGhpcy5fY3VycmVudFRhaWwpO1xuXG4gICAgdGhpcy5fYm9uZUF4aXMuY29weSh0aGlzLl9pbml0aWFsTG9jYWxDaGlsZFBvc2l0aW9uKS5ub3JtYWxpemUoKTtcbiAgICB0aGlzLl9jZW50ZXJTcGFjZUJvbmVMZW5ndGggPSBfdjNBXG4gICAgICAuY29weSh0aGlzLl9pbml0aWFsTG9jYWxDaGlsZFBvc2l0aW9uKVxuICAgICAgLmFwcGx5TWF0cml4NCh0aGlzLmJvbmUubWF0cml4V29ybGQpXG4gICAgICAuc3ViKHRoaXMuX2NlbnRlclNwYWNlUG9zaXRpb24pXG4gICAgICAubGVuZ3RoKCk7XG5cbiAgICB0aGlzLmNlbnRlciA9IHBhcmFtcy5jZW50ZXIgPz8gbnVsbDtcbiAgfVxuXG4gIC8qKlxuICAgKiBSZXNldCB0aGUgc3RhdGUgb2YgdGhpcyBib25lLlxuICAgKiBZb3UgbWlnaHQgd2FudCB0byBjYWxsIFtbVlJNU3ByaW5nQm9uZU1hbmFnZXIucmVzZXRdXSBpbnN0ZWFkLlxuICAgKi9cbiAgcHVibGljIHJlc2V0KCk6IHZvaWQge1xuICAgIHRoaXMuYm9uZS5xdWF0ZXJuaW9uLmNvcHkodGhpcy5faW5pdGlhbExvY2FsUm90YXRpb24pO1xuXG4gICAgLy8gV2UgbmVlZCB0byB1cGRhdGUgaXRzIG1hdHJpeFdvcmxkIG1hbnVhbGx5LCBzaW5jZSB3ZSB0d2Vha2VkIHRoZSBib25lIGJ5IG91ciBoYW5kXG4gICAgdGhpcy5ib25lLnVwZGF0ZU1hdHJpeCgpO1xuICAgIHRoaXMuYm9uZS5tYXRyaXhXb3JsZC5tdWx0aXBseU1hdHJpY2VzKHRoaXMuX2dldFBhcmVudE1hdHJpeFdvcmxkKCksIHRoaXMuYm9uZS5tYXRyaXgpO1xuICAgIHRoaXMuX2NlbnRlclNwYWNlUG9zaXRpb24uc2V0RnJvbU1hdHJpeFBvc2l0aW9uKHRoaXMuYm9uZS5tYXRyaXhXb3JsZCk7XG5cbiAgICAvLyBBcHBseSB1cGRhdGVkIHBvc2l0aW9uIHRvIHRhaWwgc3RhdGVzXG4gICAgdGhpcy5ib25lLmxvY2FsVG9Xb3JsZCh0aGlzLl9jdXJyZW50VGFpbC5jb3B5KHRoaXMuX2luaXRpYWxMb2NhbENoaWxkUG9zaXRpb24pKTtcbiAgICB0aGlzLl9wcmV2VGFpbC5jb3B5KHRoaXMuX2N1cnJlbnRUYWlsKTtcbiAgICB0aGlzLl9uZXh0VGFpbC5jb3B5KHRoaXMuX2N1cnJlbnRUYWlsKTtcbiAgfVxuXG4gIC8qKlxuICAgKiBVcGRhdGUgdGhlIHN0YXRlIG9mIHRoaXMgYm9uZS5cbiAgICogWW91IG1pZ2h0IHdhbnQgdG8gY2FsbCBbW1ZSTVNwcmluZ0JvbmVNYW5hZ2VyLnVwZGF0ZV1dIGluc3RlYWQuXG4gICAqXG4gICAqIEBwYXJhbSBkZWx0YSBkZWx0YVRpbWVcbiAgICovXG4gIHB1YmxpYyB1cGRhdGUoZGVsdGE6IG51bWJlcik6IHZvaWQge1xuICAgIGlmIChkZWx0YSA8PSAwKSByZXR1cm47XG5cbiAgICAvLyDopqrjgrnjg5fjg6rjg7PjgrDjg5zjg7zjg7Pjga7lp7/li6Ljga/luLjjgavlpInljJbjgZfjgabjgYTjgovjgIJcbiAgICAvLyDjgZ3jgozjgavln7rjgaXjgYTjgablh6bnkIbnm7TliY3jgavoh6rliIbjga53b3JsZE1hdHJpeOOCkuabtOaWsOOBl+OBpuOBiuOBj1xuICAgIHRoaXMuYm9uZS5tYXRyaXhXb3JsZC5tdWx0aXBseU1hdHJpY2VzKHRoaXMuX2dldFBhcmVudE1hdHJpeFdvcmxkKCksIHRoaXMuYm9uZS5tYXRyaXgpO1xuXG4gICAgaWYgKHRoaXMuYm9uZS5wYXJlbnQpIHtcbiAgICAgIC8vIFNwcmluZ0JvbmXjga/opqrjgYvjgonpoIbjgavlh6bnkIbjgZXjgozjgabjgYTjgY/jgZ/jgoHjgIFcbiAgICAgIC8vIOimquOBrm1hdHJpeFdvcmxk44Gv5pyA5paw54q25oWL44Gu5YmN5o+Q44Gnd29ybGRNYXRyaXjjgYvjgolxdWF0ZXJuaW9u44KS5Y+W44KK5Ye644GZ44CCXG4gICAgICAvLyDliLbpmZDjga/jgYLjgovjgZHjgozjganjgIHoqIjnrpfjga/lsJHjgarjgYTjga7jgadnZXRXb3JsZFF1YXRlcm5pb27jgafjga/jgarjgY/jgZPjga7mlrnms5XjgpLlj5bjgovjgIJcbiAgICAgIGdldFdvcmxkUXVhdGVybmlvbkxpdGUodGhpcy5ib25lLnBhcmVudCwgdGhpcy5fcGFyZW50V29ybGRSb3RhdGlvbik7XG4gICAgfSBlbHNlIHtcbiAgICAgIHRoaXMuX3BhcmVudFdvcmxkUm90YXRpb24uY29weShJREVOVElUWV9RVUFURVJOSU9OKTtcbiAgICB9XG5cbiAgICAvLyBHZXQgYm9uZSBwb3NpdGlvbiBpbiBjZW50ZXIgc3BhY2VcbiAgICB0aGlzLl9nZXRNYXRyaXhXb3JsZFRvQ2VudGVyKF9tYXRBKTtcbiAgICBfbWF0QS5tdWx0aXBseSh0aGlzLmJvbmUubWF0cml4V29ybGQpOyAvLyDwn5SlID8/XG4gICAgdGhpcy5fY2VudGVyU3BhY2VQb3NpdGlvbi5zZXRGcm9tTWF0cml4UG9zaXRpb24oX21hdEEpO1xuXG4gICAgLy8gR2V0IHBhcmVudCBwb3NpdGlvbiBpbiBjZW50ZXIgc3BhY2VcbiAgICB0aGlzLl9nZXRNYXRyaXhXb3JsZFRvQ2VudGVyKF9tYXRCKTtcbiAgICBfbWF0Qi5tdWx0aXBseSh0aGlzLl9nZXRQYXJlbnRNYXRyaXhXb3JsZCgpKTtcblxuICAgIC8vIHNldmVyYWwgcGFyYW1ldGVyc1xuICAgIGNvbnN0IHN0aWZmbmVzcyA9IHRoaXMuc3RpZmZuZXNzRm9yY2UgKiBkZWx0YTtcbiAgICBjb25zdCBleHRlcm5hbCA9IF92M0IuY29weSh0aGlzLmdyYXZpdHlEaXIpLm11bHRpcGx5U2NhbGFyKHRoaXMuZ3Jhdml0eVBvd2VyICogZGVsdGEpO1xuXG4gICAgLy8gdmVybGV056mN5YiG44Gn5qyh44Gu5L2N572u44KS6KiI566XXG4gICAgdGhpcy5fbmV4dFRhaWxcbiAgICAgIC5jb3B5KHRoaXMuX2N1cnJlbnRUYWlsKVxuICAgICAgLmFkZChcbiAgICAgICAgX3YzQVxuICAgICAgICAgIC5jb3B5KHRoaXMuX2N1cnJlbnRUYWlsKVxuICAgICAgICAgIC5zdWIodGhpcy5fcHJldlRhaWwpXG4gICAgICAgICAgLm11bHRpcGx5U2NhbGFyKDEgLSB0aGlzLmRyYWdGb3JjZSksXG4gICAgICApIC8vIOWJjeODleODrOODvOODoOOBruenu+WLleOCkue2mee2muOBmeOCiyjmuJvoobDjgoLjgYLjgovjgogpXG4gICAgICAuYWRkKFxuICAgICAgICBfdjNBXG4gICAgICAgICAgLmNvcHkodGhpcy5fYm9uZUF4aXMpXG4gICAgICAgICAgLmFwcGx5TWF0cml4NCh0aGlzLl9pbml0aWFsTG9jYWxNYXRyaXgpXG4gICAgICAgICAgLmFwcGx5TWF0cml4NChfbWF0QilcbiAgICAgICAgICAuc3ViKHRoaXMuX2NlbnRlclNwYWNlUG9zaXRpb24pXG4gICAgICAgICAgLm5vcm1hbGl6ZSgpXG4gICAgICAgICAgLm11bHRpcGx5U2NhbGFyKHN0aWZmbmVzcyksXG4gICAgICApIC8vIOimquOBruWbnui7ouOBq+OCiOOCi+WtkOODnOODvOODs+OBruenu+WLleebruaomVxuICAgICAgLmFkZChleHRlcm5hbCk7IC8vIOWkluWKm+OBq+OCiOOCi+enu+WLlemHj1xuXG4gICAgLy8gbm9ybWFsaXplIGJvbmUgbGVuZ3RoXG4gICAgdGhpcy5fbmV4dFRhaWxcbiAgICAgIC5zdWIodGhpcy5fY2VudGVyU3BhY2VQb3NpdGlvbilcbiAgICAgIC5ub3JtYWxpemUoKVxuICAgICAgLm11bHRpcGx5U2NhbGFyKHRoaXMuX2NlbnRlclNwYWNlQm9uZUxlbmd0aClcbiAgICAgIC5hZGQodGhpcy5fY2VudGVyU3BhY2VQb3NpdGlvbik7XG5cbiAgICAvLyBDb2xsaXNpb27jgafnp7vli5VcbiAgICB0aGlzLl9jb2xsaXNpb24odGhpcy5fbmV4dFRhaWwpO1xuXG4gICAgdGhpcy5fcHJldlRhaWwuY29weSh0aGlzLl9jdXJyZW50VGFpbCk7XG4gICAgdGhpcy5fY3VycmVudFRhaWwuY29weSh0aGlzLl9uZXh0VGFpbCk7XG5cbiAgICAvLyBBcHBseSByb3RhdGlvbiwgY29udmVydCB2ZWN0b3IzIHRoaW5nIGludG8gYWN0dWFsIHF1YXRlcm5pb25cbiAgICAvLyBPcmlnaW5hbCBVbmlWUk0gaXMgZG9pbmcgd29ybGQgdW5pdCBjYWxjdWx1cyBhdCBoZXJlIGJ1dCB3ZSdyZSBnb25uYSBkbyB0aGlzIG9uIGxvY2FsIHVuaXRcbiAgICAvLyBzaW5jZSBUaHJlZS5qcyBpcyBub3QgZ29vZCBhdCB3b3JsZCBjb29yZGluYXRpb24gc3R1ZmZcbiAgICBjb25zdCBpbml0aWFsQ2VudGVyU3BhY2VNYXRyaXhJbnYgPSBtYXQ0SW52ZXJ0Q29tcGF0KF9tYXRBLmNvcHkoX21hdEIubXVsdGlwbHkodGhpcy5faW5pdGlhbExvY2FsTWF0cml4KSkpO1xuICAgIGNvbnN0IGFwcGx5Um90YXRpb24gPSBfcXVhdEEuc2V0RnJvbVVuaXRWZWN0b3JzKFxuICAgICAgdGhpcy5fYm9uZUF4aXMsXG4gICAgICBfdjNBLmNvcHkodGhpcy5fbmV4dFRhaWwpLmFwcGx5TWF0cml4NChpbml0aWFsQ2VudGVyU3BhY2VNYXRyaXhJbnYpLm5vcm1hbGl6ZSgpLFxuICAgICk7XG5cbiAgICB0aGlzLmJvbmUucXVhdGVybmlvbi5jb3B5KHRoaXMuX2luaXRpYWxMb2NhbFJvdGF0aW9uKS5tdWx0aXBseShhcHBseVJvdGF0aW9uKTtcblxuICAgIC8vIFdlIG5lZWQgdG8gdXBkYXRlIGl0cyBtYXRyaXhXb3JsZCBtYW51YWxseSwgc2luY2Ugd2UgdHdlYWtlZCB0aGUgYm9uZSBieSBvdXIgaGFuZFxuICAgIHRoaXMuYm9uZS51cGRhdGVNYXRyaXgoKTtcbiAgICB0aGlzLmJvbmUubWF0cml4V29ybGQubXVsdGlwbHlNYXRyaWNlcyh0aGlzLl9nZXRQYXJlbnRNYXRyaXhXb3JsZCgpLCB0aGlzLmJvbmUubWF0cml4KTtcbiAgfVxuXG4gIC8qKlxuICAgKiBEbyBjb2xsaXNpb24gbWF0aCBhZ2FpbnN0IGV2ZXJ5IGNvbGxpZGVycyBhdHRhY2hlZCB0byB0aGlzIGJvbmUuXG4gICAqXG4gICAqIEBwYXJhbSB0YWlsIFRoZSB0YWlsIHlvdSB3YW50IHRvIHByb2Nlc3NcbiAgICovXG4gIHByaXZhdGUgX2NvbGxpc2lvbih0YWlsOiBUSFJFRS5WZWN0b3IzKTogdm9pZCB7XG4gICAgdGhpcy5jb2xsaWRlcnMuZm9yRWFjaCgoY29sbGlkZXIpID0+IHtcbiAgICAgIHRoaXMuX2dldE1hdHJpeFdvcmxkVG9DZW50ZXIoX21hdEEpO1xuICAgICAgX21hdEEubXVsdGlwbHkoY29sbGlkZXIubWF0cml4V29ybGQpO1xuICAgICAgY29uc3QgY29sbGlkZXJDZW50ZXJTcGFjZVBvc2l0aW9uID0gX3YzQS5zZXRGcm9tTWF0cml4UG9zaXRpb24oX21hdEEpO1xuICAgICAgY29uc3QgY29sbGlkZXJSYWRpdXMgPSBjb2xsaWRlci5nZW9tZXRyeS5ib3VuZGluZ1NwaGVyZSEucmFkaXVzOyAvLyB0aGUgYm91bmRpbmcgc3BoZXJlIGlzIGd1YXJhbnRlZWQgdG8gYmUgZXhpc3QgYnkgVlJNU3ByaW5nQm9uZUltcG9ydGVyLl9jcmVhdGVDb2xsaWRlck1lc2hcbiAgICAgIGNvbnN0IHIgPSB0aGlzLnJhZGl1cyArIGNvbGxpZGVyUmFkaXVzO1xuXG4gICAgICBpZiAodGFpbC5kaXN0YW5jZVRvU3F1YXJlZChjb2xsaWRlckNlbnRlclNwYWNlUG9zaXRpb24pIDw9IHIgKiByKSB7XG4gICAgICAgIC8vIOODkuODg+ODiOOAgkNvbGxpZGVy44Gu5Y2K5b6E5pa55ZCR44Gr5oq844GX5Ye644GZXG4gICAgICAgIGNvbnN0IG5vcm1hbCA9IF92M0Iuc3ViVmVjdG9ycyh0YWlsLCBjb2xsaWRlckNlbnRlclNwYWNlUG9zaXRpb24pLm5vcm1hbGl6ZSgpO1xuICAgICAgICBjb25zdCBwb3NGcm9tQ29sbGlkZXIgPSBfdjNDLmFkZFZlY3RvcnMoY29sbGlkZXJDZW50ZXJTcGFjZVBvc2l0aW9uLCBub3JtYWwubXVsdGlwbHlTY2FsYXIocikpO1xuXG4gICAgICAgIC8vIG5vcm1hbGl6ZSBib25lIGxlbmd0aFxuICAgICAgICB0YWlsLmNvcHkoXG4gICAgICAgICAgcG9zRnJvbUNvbGxpZGVyXG4gICAgICAgICAgICAuc3ViKHRoaXMuX2NlbnRlclNwYWNlUG9zaXRpb24pXG4gICAgICAgICAgICAubm9ybWFsaXplKClcbiAgICAgICAgICAgIC5tdWx0aXBseVNjYWxhcih0aGlzLl9jZW50ZXJTcGFjZUJvbmVMZW5ndGgpXG4gICAgICAgICAgICAuYWRkKHRoaXMuX2NlbnRlclNwYWNlUG9zaXRpb24pLFxuICAgICAgICApO1xuICAgICAgfVxuICAgIH0pO1xuICB9XG5cbiAgLyoqXG4gICAqIENyZWF0ZSBhIG1hdHJpeCB0aGF0IGNvbnZlcnRzIGNlbnRlciBzcGFjZSBpbnRvIHdvcmxkIHNwYWNlLlxuICAgKiBAcGFyYW0gdGFyZ2V0IFRhcmdldCBtYXRyaXhcbiAgICovXG4gIHByaXZhdGUgX2dldE1hdHJpeENlbnRlclRvV29ybGQodGFyZ2V0OiBUSFJFRS5NYXRyaXg0KTogVEhSRUUuTWF0cml4NCB7XG4gICAgaWYgKHRoaXMuX2NlbnRlcikge1xuICAgICAgdGFyZ2V0LmNvcHkodGhpcy5fY2VudGVyLm1hdHJpeFdvcmxkKTtcbiAgICB9IGVsc2Uge1xuICAgICAgdGFyZ2V0LmlkZW50aXR5KCk7XG4gICAgfVxuXG4gICAgcmV0dXJuIHRhcmdldDtcbiAgfVxuXG4gIC8qKlxuICAgKiBDcmVhdGUgYSBtYXRyaXggdGhhdCBjb252ZXJ0cyB3b3JsZCBzcGFjZSBpbnRvIGNlbnRlciBzcGFjZS5cbiAgICogQHBhcmFtIHRhcmdldCBUYXJnZXQgbWF0cml4XG4gICAqL1xuICBwcml2YXRlIF9nZXRNYXRyaXhXb3JsZFRvQ2VudGVyKHRhcmdldDogVEhSRUUuTWF0cml4NCk6IFRIUkVFLk1hdHJpeDQge1xuICAgIGlmICh0aGlzLl9jZW50ZXIpIHtcbiAgICAgIHRhcmdldC5jb3B5KCh0aGlzLl9jZW50ZXIudXNlckRhdGEuaW52ZXJzZUNhY2hlUHJveHkgYXMgTWF0cml4NEludmVyc2VDYWNoZSkuaW52ZXJzZSk7XG4gICAgfSBlbHNlIHtcbiAgICAgIHRhcmdldC5pZGVudGl0eSgpO1xuICAgIH1cblxuICAgIHJldHVybiB0YXJnZXQ7XG4gIH1cblxuICAvKipcbiAgICogUmV0dXJucyB0aGUgd29ybGQgbWF0cml4IG9mIGl0cyBwYXJlbnQgb2JqZWN0LlxuICAgKi9cbiAgcHJpdmF0ZSBfZ2V0UGFyZW50TWF0cml4V29ybGQoKTogVEhSRUUuTWF0cml4NCB7XG4gICAgcmV0dXJuIHRoaXMuYm9uZS5wYXJlbnQgPyB0aGlzLmJvbmUucGFyZW50Lm1hdHJpeFdvcmxkIDogSURFTlRJVFlfTUFUUklYNDtcbiAgfVxufVxuIiwiaW1wb3J0IHsgVlJNU3ByaW5nQm9uZSB9IGZyb20gJy4vVlJNU3ByaW5nQm9uZSc7XG5pbXBvcnQgeyBWUk1TcHJpbmdCb25lQ29sbGlkZXJHcm91cCB9IGZyb20gJy4vVlJNU3ByaW5nQm9uZUNvbGxpZGVyR3JvdXAnO1xuXG4vKipcbiAqIFJlcHJlc2VudHMgYSBzaW5nbGUgc3ByaW5nIGJvbmUgZ3JvdXAgb2YgYSBWUk0uXG4gKi9cbmV4cG9ydCB0eXBlIFZSTVNwcmluZ0JvbmVHcm91cCA9IFZSTVNwcmluZ0JvbmVbXTtcblxuLyoqXG4gKiBBIGNsYXNzIG1hbmFnZXMgZXZlcnkgc3ByaW5nIGJvbmVzIG9uIGEgVlJNLlxuICovXG5leHBvcnQgY2xhc3MgVlJNU3ByaW5nQm9uZU1hbmFnZXIge1xuICBwdWJsaWMgcmVhZG9ubHkgY29sbGlkZXJHcm91cHM6IFZSTVNwcmluZ0JvbmVDb2xsaWRlckdyb3VwW10gPSBbXTtcbiAgcHVibGljIHJlYWRvbmx5IHNwcmluZ0JvbmVHcm91cExpc3Q6IFZSTVNwcmluZ0JvbmVHcm91cFtdID0gW107XG5cbiAgLyoqXG4gICAqIENyZWF0ZSBhIG5ldyBbW1ZSTVNwcmluZ0JvbmVNYW5hZ2VyXV1cbiAgICpcbiAgICogQHBhcmFtIHNwcmluZ0JvbmVHcm91cExpc3QgQW4gYXJyYXkgb2YgW1tWUk1TcHJpbmdCb25lR3JvdXBdXVxuICAgKi9cbiAgcHVibGljIGNvbnN0cnVjdG9yKGNvbGxpZGVyR3JvdXBzOiBWUk1TcHJpbmdCb25lQ29sbGlkZXJHcm91cFtdLCBzcHJpbmdCb25lR3JvdXBMaXN0OiBWUk1TcHJpbmdCb25lR3JvdXBbXSkge1xuICAgIHRoaXMuY29sbGlkZXJHcm91cHMgPSBjb2xsaWRlckdyb3VwcztcbiAgICB0aGlzLnNwcmluZ0JvbmVHcm91cExpc3QgPSBzcHJpbmdCb25lR3JvdXBMaXN0O1xuICB9XG5cbiAgLyoqXG4gICAqIFNldCBhbGwgYm9uZXMgYmUgY2FsY3VsYXRlZCBiYXNlZCBvbiB0aGUgc3BhY2UgcmVsYXRpdmUgZnJvbSB0aGlzIG9iamVjdC5cbiAgICogSWYgYG51bGxgIGlzIGdpdmVuLCBzcHJpbmdib25lIHdpbGwgYmUgY2FsY3VsYXRlZCBpbiB3b3JsZCBzcGFjZS5cbiAgICogQHBhcmFtIHJvb3QgUm9vdCBvYmplY3QsIG9yIGBudWxsYFxuICAgKi9cbiAgcHVibGljIHNldENlbnRlcihyb290OiBUSFJFRS5PYmplY3QzRCB8IG51bGwpOiB2b2lkIHtcbiAgICB0aGlzLnNwcmluZ0JvbmVHcm91cExpc3QuZm9yRWFjaCgoc3ByaW5nQm9uZUdyb3VwKSA9PiB7XG4gICAgICBzcHJpbmdCb25lR3JvdXAuZm9yRWFjaCgoc3ByaW5nQm9uZSkgPT4ge1xuICAgICAgICBzcHJpbmdCb25lLmNlbnRlciA9IHJvb3Q7XG4gICAgICB9KTtcbiAgICB9KTtcbiAgfVxuXG4gIC8qKlxuICAgKiBVcGRhdGUgZXZlcnkgc3ByaW5nIGJvbmUgYXR0YWNoZWQgdG8gdGhpcyBtYW5hZ2VyLlxuICAgKlxuICAgKiBAcGFyYW0gZGVsdGEgZGVsdGFUaW1lXG4gICAqL1xuICBwdWJsaWMgbGF0ZVVwZGF0ZShkZWx0YTogbnVtYmVyKTogdm9pZCB7XG4gICAgdGhpcy5zcHJpbmdCb25lR3JvdXBMaXN0LmZvckVhY2goKHNwcmluZ0JvbmVHcm91cCkgPT4ge1xuICAgICAgc3ByaW5nQm9uZUdyb3VwLmZvckVhY2goKHNwcmluZ0JvbmUpID0+IHtcbiAgICAgICAgc3ByaW5nQm9uZS51cGRhdGUoZGVsdGEpO1xuICAgICAgfSk7XG4gICAgfSk7XG4gIH1cblxuICAvKipcbiAgICogUmVzZXQgZXZlcnkgc3ByaW5nIGJvbmUgYXR0YWNoZWQgdG8gdGhpcyBtYW5hZ2VyLlxuICAgKi9cbiAgcHVibGljIHJlc2V0KCk6IHZvaWQge1xuICAgIHRoaXMuc3ByaW5nQm9uZUdyb3VwTGlzdC5mb3JFYWNoKChzcHJpbmdCb25lR3JvdXApID0+IHtcbiAgICAgIHNwcmluZ0JvbmVHcm91cC5mb3JFYWNoKChzcHJpbmdCb25lKSA9PiB7XG4gICAgICAgIHNwcmluZ0JvbmUucmVzZXQoKTtcbiAgICAgIH0pO1xuICAgIH0pO1xuICB9XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5pbXBvcnQgeyBHTFRGIH0gZnJvbSAndGhyZWUvZXhhbXBsZXMvanNtL2xvYWRlcnMvR0xURkxvYWRlcic7XG5pbXBvcnQgeyBHTFRGTm9kZSwgVlJNU2NoZW1hIH0gZnJvbSAnLi4vdHlwZXMnO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZSB9IGZyb20gJy4vVlJNU3ByaW5nQm9uZSc7XG5pbXBvcnQgeyBWUk1TcHJpbmdCb25lQ29sbGlkZXJHcm91cCwgVlJNU3ByaW5nQm9uZUNvbGxpZGVyTWVzaCB9IGZyb20gJy4vVlJNU3ByaW5nQm9uZUNvbGxpZGVyR3JvdXAnO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZUdyb3VwLCBWUk1TcHJpbmdCb25lTWFuYWdlciB9IGZyb20gJy4vVlJNU3ByaW5nQm9uZU1hbmFnZXInO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZVBhcmFtZXRlcnMgfSBmcm9tICcuL1ZSTVNwcmluZ0JvbmVQYXJhbWV0ZXJzJztcblxuY29uc3QgX3YzQSA9IG5ldyBUSFJFRS5WZWN0b3IzKCk7XG5cbmNvbnN0IF9jb2xsaWRlck1hdGVyaWFsID0gbmV3IFRIUkVFLk1lc2hCYXNpY01hdGVyaWFsKHsgdmlzaWJsZTogZmFsc2UgfSk7XG5cbi8qKlxuICogQW4gaW1wb3J0ZXIgdGhhdCBpbXBvcnRzIGEgW1tWUk1TcHJpbmdCb25lTWFuYWdlcl1dIGZyb20gYSBWUk0gZXh0ZW5zaW9uIG9mIGEgR0xURi5cbiAqL1xuZXhwb3J0IGNsYXNzIFZSTVNwcmluZ0JvbmVJbXBvcnRlciB7XG4gIC8qKlxuICAgKiBJbXBvcnQgYSBbW1ZSTUxvb2tBdEhlYWRdXSBmcm9tIGEgVlJNLlxuICAgKlxuICAgKiBAcGFyYW0gZ2x0ZiBBIHBhcnNlZCByZXN1bHQgb2YgR0xURiB0YWtlbiBmcm9tIEdMVEZMb2FkZXJcbiAgICovXG4gIHB1YmxpYyBhc3luYyBpbXBvcnQoZ2x0ZjogR0xURik6IFByb21pc2U8VlJNU3ByaW5nQm9uZU1hbmFnZXIgfCBudWxsPiB7XG4gICAgY29uc3QgdnJtRXh0OiBWUk1TY2hlbWEuVlJNIHwgdW5kZWZpbmVkID0gZ2x0Zi5wYXJzZXIuanNvbi5leHRlbnNpb25zPy5WUk07XG4gICAgaWYgKCF2cm1FeHQpIHJldHVybiBudWxsO1xuXG4gICAgY29uc3Qgc2NoZW1hU2Vjb25kYXJ5QW5pbWF0aW9uOiBWUk1TY2hlbWEuU2Vjb25kYXJ5QW5pbWF0aW9uIHwgdW5kZWZpbmVkID0gdnJtRXh0LnNlY29uZGFyeUFuaW1hdGlvbjtcbiAgICBpZiAoIXNjaGVtYVNlY29uZGFyeUFuaW1hdGlvbikgcmV0dXJuIG51bGw7XG5cbiAgICAvLyDooZ3nqoHliKTlrprnkIPkvZPjg6Hjg4Pjgrfjg6XjgIJcbiAgICBjb25zdCBjb2xsaWRlckdyb3VwcyA9IGF3YWl0IHRoaXMuX2ltcG9ydENvbGxpZGVyTWVzaEdyb3VwcyhnbHRmLCBzY2hlbWFTZWNvbmRhcnlBbmltYXRpb24pO1xuXG4gICAgLy8g5ZCM44GY5bGe5oCn77yIc3RpZmZpbmVzc+OChGRyYWdGb3JjZeOBjOWQjOOBmO+8ieOBruODnOODvOODs+OBr2JvbmVHcm91cOOBq+OBvuOBqOOCgeOCieOCjOOBpuOBhOOCi+OAglxuICAgIC8vIOS4gOWIl+OBoOOBkeOBp+OBr+OBquOBhOOBk+OBqOOBq+azqOaEj+OAglxuICAgIGNvbnN0IHNwcmluZ0JvbmVHcm91cExpc3QgPSBhd2FpdCB0aGlzLl9pbXBvcnRTcHJpbmdCb25lR3JvdXBMaXN0KGdsdGYsIHNjaGVtYVNlY29uZGFyeUFuaW1hdGlvbiwgY29sbGlkZXJHcm91cHMpO1xuXG4gICAgcmV0dXJuIG5ldyBWUk1TcHJpbmdCb25lTWFuYWdlcihjb2xsaWRlckdyb3Vwcywgc3ByaW5nQm9uZUdyb3VwTGlzdCk7XG4gIH1cblxuICBwcm90ZWN0ZWQgX2NyZWF0ZVNwcmluZ0JvbmUoYm9uZTogVEhSRUUuT2JqZWN0M0QsIHBhcmFtczogVlJNU3ByaW5nQm9uZVBhcmFtZXRlcnMgPSB7fSk6IFZSTVNwcmluZ0JvbmUge1xuICAgIHJldHVybiBuZXcgVlJNU3ByaW5nQm9uZShib25lLCBwYXJhbXMpO1xuICB9XG5cbiAgcHJvdGVjdGVkIGFzeW5jIF9pbXBvcnRTcHJpbmdCb25lR3JvdXBMaXN0KFxuICAgIGdsdGY6IEdMVEYsXG4gICAgc2NoZW1hU2Vjb25kYXJ5QW5pbWF0aW9uOiBWUk1TY2hlbWEuU2Vjb25kYXJ5QW5pbWF0aW9uLFxuICAgIGNvbGxpZGVyR3JvdXBzOiBWUk1TcHJpbmdCb25lQ29sbGlkZXJHcm91cFtdLFxuICApOiBQcm9taXNlPFZSTVNwcmluZ0JvbmVHcm91cFtdPiB7XG4gICAgY29uc3Qgc3ByaW5nQm9uZUdyb3VwczogVlJNU2NoZW1hLlNlY29uZGFyeUFuaW1hdGlvblNwcmluZ1tdID0gc2NoZW1hU2Vjb25kYXJ5QW5pbWF0aW9uLmJvbmVHcm91cHMgfHwgW107XG5cbiAgICBjb25zdCBzcHJpbmdCb25lR3JvdXBMaXN0OiBWUk1TcHJpbmdCb25lR3JvdXBbXSA9IFtdO1xuXG4gICAgYXdhaXQgUHJvbWlzZS5hbGwoXG4gICAgICBzcHJpbmdCb25lR3JvdXBzLm1hcChhc3luYyAodnJtQm9uZUdyb3VwKSA9PiB7XG4gICAgICAgIGlmIChcbiAgICAgICAgICB2cm1Cb25lR3JvdXAuc3RpZmZpbmVzcyA9PT0gdW5kZWZpbmVkIHx8XG4gICAgICAgICAgdnJtQm9uZUdyb3VwLmdyYXZpdHlEaXIgPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIHZybUJvbmVHcm91cC5ncmF2aXR5RGlyLnggPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIHZybUJvbmVHcm91cC5ncmF2aXR5RGlyLnkgPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIHZybUJvbmVHcm91cC5ncmF2aXR5RGlyLnogPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIHZybUJvbmVHcm91cC5ncmF2aXR5UG93ZXIgPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIHZybUJvbmVHcm91cC5kcmFnRm9yY2UgPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIHZybUJvbmVHcm91cC5oaXRSYWRpdXMgPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIHZybUJvbmVHcm91cC5jb2xsaWRlckdyb3VwcyA9PT0gdW5kZWZpbmVkIHx8XG4gICAgICAgICAgdnJtQm9uZUdyb3VwLmJvbmVzID09PSB1bmRlZmluZWQgfHxcbiAgICAgICAgICB2cm1Cb25lR3JvdXAuY2VudGVyID09PSB1bmRlZmluZWRcbiAgICAgICAgKSB7XG4gICAgICAgICAgcmV0dXJuO1xuICAgICAgICB9XG5cbiAgICAgICAgY29uc3Qgc3RpZmZuZXNzRm9yY2UgPSB2cm1Cb25lR3JvdXAuc3RpZmZpbmVzcztcbiAgICAgICAgY29uc3QgZ3Jhdml0eURpciA9IG5ldyBUSFJFRS5WZWN0b3IzKFxuICAgICAgICAgIHZybUJvbmVHcm91cC5ncmF2aXR5RGlyLngsXG4gICAgICAgICAgdnJtQm9uZUdyb3VwLmdyYXZpdHlEaXIueSxcbiAgICAgICAgICAtdnJtQm9uZUdyb3VwLmdyYXZpdHlEaXIueiwgLy8gVlJNIDAuMCB1c2VzIGxlZnQtaGFuZGVkIHktdXBcbiAgICAgICAgKTtcbiAgICAgICAgY29uc3QgZ3Jhdml0eVBvd2VyID0gdnJtQm9uZUdyb3VwLmdyYXZpdHlQb3dlcjtcbiAgICAgICAgY29uc3QgZHJhZ0ZvcmNlID0gdnJtQm9uZUdyb3VwLmRyYWdGb3JjZTtcbiAgICAgICAgY29uc3QgcmFkaXVzID0gdnJtQm9uZUdyb3VwLmhpdFJhZGl1cztcblxuICAgICAgICBjb25zdCBjb2xsaWRlcnM6IFZSTVNwcmluZ0JvbmVDb2xsaWRlck1lc2hbXSA9IFtdO1xuICAgICAgICB2cm1Cb25lR3JvdXAuY29sbGlkZXJHcm91cHMuZm9yRWFjaCgoY29sbGlkZXJJbmRleCkgPT4ge1xuICAgICAgICAgIGNvbGxpZGVycy5wdXNoKC4uLmNvbGxpZGVyR3JvdXBzW2NvbGxpZGVySW5kZXhdLmNvbGxpZGVycyk7XG4gICAgICAgIH0pO1xuXG4gICAgICAgIGNvbnN0IHNwcmluZ0JvbmVHcm91cDogVlJNU3ByaW5nQm9uZUdyb3VwID0gW107XG4gICAgICAgIGF3YWl0IFByb21pc2UuYWxsKFxuICAgICAgICAgIHZybUJvbmVHcm91cC5ib25lcy5tYXAoYXN5bmMgKG5vZGVJbmRleCkgPT4ge1xuICAgICAgICAgICAgLy8gVlJN44Gu5oOF5aCx44GL44KJ44CM5o+644KM44Oi44OO44CN44Oc44O844Oz44Gu44Or44O844OI44GM5Y+W44KM44KLXG4gICAgICAgICAgICBjb25zdCBzcHJpbmdSb290Qm9uZTogR0xURk5vZGUgPSBhd2FpdCBnbHRmLnBhcnNlci5nZXREZXBlbmRlbmN5KCdub2RlJywgbm9kZUluZGV4KTtcblxuICAgICAgICAgICAgY29uc3QgY2VudGVyOiBHTFRGTm9kZSA9XG4gICAgICAgICAgICAgIHZybUJvbmVHcm91cC5jZW50ZXIhICE9PSAtMSA/IGF3YWl0IGdsdGYucGFyc2VyLmdldERlcGVuZGVuY3koJ25vZGUnLCB2cm1Cb25lR3JvdXAuY2VudGVyISkgOiBudWxsO1xuXG4gICAgICAgICAgICAvLyBpdCdzIHdlaXJkIGJ1dCB0aGVyZSBtaWdodCBiZSBjYXNlcyB3ZSBjYW4ndCBmaW5kIHRoZSByb290IGJvbmVcbiAgICAgICAgICAgIGlmICghc3ByaW5nUm9vdEJvbmUpIHtcbiAgICAgICAgICAgICAgcmV0dXJuO1xuICAgICAgICAgICAgfVxuXG4gICAgICAgICAgICBzcHJpbmdSb290Qm9uZS50cmF2ZXJzZSgoYm9uZSkgPT4ge1xuICAgICAgICAgICAgICBjb25zdCBzcHJpbmdCb25lID0gdGhpcy5fY3JlYXRlU3ByaW5nQm9uZShib25lLCB7XG4gICAgICAgICAgICAgICAgcmFkaXVzLFxuICAgICAgICAgICAgICAgIHN0aWZmbmVzc0ZvcmNlLFxuICAgICAgICAgICAgICAgIGdyYXZpdHlEaXIsXG4gICAgICAgICAgICAgICAgZ3Jhdml0eVBvd2VyLFxuICAgICAgICAgICAgICAgIGRyYWdGb3JjZSxcbiAgICAgICAgICAgICAgICBjb2xsaWRlcnMsXG4gICAgICAgICAgICAgICAgY2VudGVyLFxuICAgICAgICAgICAgICB9KTtcbiAgICAgICAgICAgICAgc3ByaW5nQm9uZUdyb3VwLnB1c2goc3ByaW5nQm9uZSk7XG4gICAgICAgICAgICB9KTtcbiAgICAgICAgICB9KSxcbiAgICAgICAgKTtcblxuICAgICAgICBzcHJpbmdCb25lR3JvdXBMaXN0LnB1c2goc3ByaW5nQm9uZUdyb3VwKTtcbiAgICAgIH0pLFxuICAgICk7XG5cbiAgICByZXR1cm4gc3ByaW5nQm9uZUdyb3VwTGlzdDtcbiAgfVxuXG4gIC8qKlxuICAgKiBDcmVhdGUgYW4gYXJyYXkgb2YgW1tWUk1TcHJpbmdCb25lQ29sbGlkZXJHcm91cF1dLlxuICAgKlxuICAgKiBAcGFyYW0gZ2x0ZiBBIHBhcnNlZCByZXN1bHQgb2YgR0xURiB0YWtlbiBmcm9tIEdMVEZMb2FkZXJcbiAgICogQHBhcmFtIHNjaGVtYVNlY29uZGFyeUFuaW1hdGlvbiBBIGBzZWNvbmRhcnlBbmltYXRpb25gIGZpZWxkIG9mIFZSTVxuICAgKi9cbiAgcHJvdGVjdGVkIGFzeW5jIF9pbXBvcnRDb2xsaWRlck1lc2hHcm91cHMoXG4gICAgZ2x0ZjogR0xURixcbiAgICBzY2hlbWFTZWNvbmRhcnlBbmltYXRpb246IFZSTVNjaGVtYS5TZWNvbmRhcnlBbmltYXRpb24sXG4gICk6IFByb21pc2U8VlJNU3ByaW5nQm9uZUNvbGxpZGVyR3JvdXBbXT4ge1xuICAgIGNvbnN0IHZybUNvbGxpZGVyR3JvdXBzID0gc2NoZW1hU2Vjb25kYXJ5QW5pbWF0aW9uLmNvbGxpZGVyR3JvdXBzO1xuICAgIGlmICh2cm1Db2xsaWRlckdyb3VwcyA9PT0gdW5kZWZpbmVkKSByZXR1cm4gW107XG5cbiAgICBjb25zdCBjb2xsaWRlckdyb3VwczogVlJNU3ByaW5nQm9uZUNvbGxpZGVyR3JvdXBbXSA9IFtdO1xuICAgIHZybUNvbGxpZGVyR3JvdXBzLmZvckVhY2goYXN5bmMgKGNvbGxpZGVyR3JvdXApID0+IHtcbiAgICAgIGlmIChjb2xsaWRlckdyb3VwLm5vZGUgPT09IHVuZGVmaW5lZCB8fCBjb2xsaWRlckdyb3VwLmNvbGxpZGVycyA9PT0gdW5kZWZpbmVkKSB7XG4gICAgICAgIHJldHVybjtcbiAgICAgIH1cblxuICAgICAgY29uc3QgYm9uZSA9IGF3YWl0IGdsdGYucGFyc2VyLmdldERlcGVuZGVuY3koJ25vZGUnLCBjb2xsaWRlckdyb3VwLm5vZGUpO1xuICAgICAgY29uc3QgY29sbGlkZXJzOiBWUk1TcHJpbmdCb25lQ29sbGlkZXJNZXNoW10gPSBbXTtcbiAgICAgIGNvbGxpZGVyR3JvdXAuY29sbGlkZXJzLmZvckVhY2goKGNvbGxpZGVyKSA9PiB7XG4gICAgICAgIGlmIChcbiAgICAgICAgICBjb2xsaWRlci5vZmZzZXQgPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIGNvbGxpZGVyLm9mZnNldC54ID09PSB1bmRlZmluZWQgfHxcbiAgICAgICAgICBjb2xsaWRlci5vZmZzZXQueSA9PT0gdW5kZWZpbmVkIHx8XG4gICAgICAgICAgY29sbGlkZXIub2Zmc2V0LnogPT09IHVuZGVmaW5lZCB8fFxuICAgICAgICAgIGNvbGxpZGVyLnJhZGl1cyA9PT0gdW5kZWZpbmVkXG4gICAgICAgICkge1xuICAgICAgICAgIHJldHVybjtcbiAgICAgICAgfVxuXG4gICAgICAgIGNvbnN0IG9mZnNldCA9IF92M0Euc2V0KFxuICAgICAgICAgIGNvbGxpZGVyLm9mZnNldC54LFxuICAgICAgICAgIGNvbGxpZGVyLm9mZnNldC55LFxuICAgICAgICAgIC1jb2xsaWRlci5vZmZzZXQueiwgLy8gVlJNIDAuMCB1c2VzIGxlZnQtaGFuZGVkIHktdXBcbiAgICAgICAgKTtcbiAgICAgICAgY29uc3QgY29sbGlkZXJNZXNoID0gdGhpcy5fY3JlYXRlQ29sbGlkZXJNZXNoKGNvbGxpZGVyLnJhZGl1cywgb2Zmc2V0KTtcblxuICAgICAgICBib25lLmFkZChjb2xsaWRlck1lc2gpO1xuICAgICAgICBjb2xsaWRlcnMucHVzaChjb2xsaWRlck1lc2gpO1xuICAgICAgfSk7XG5cbiAgICAgIGNvbnN0IGNvbGxpZGVyTWVzaEdyb3VwID0ge1xuICAgICAgICBub2RlOiBjb2xsaWRlckdyb3VwLm5vZGUsXG4gICAgICAgIGNvbGxpZGVycyxcbiAgICAgIH07XG4gICAgICBjb2xsaWRlckdyb3Vwcy5wdXNoKGNvbGxpZGVyTWVzaEdyb3VwKTtcbiAgICB9KTtcblxuICAgIHJldHVybiBjb2xsaWRlckdyb3VwcztcbiAgfVxuXG4gIC8qKlxuICAgKiBDcmVhdGUgYSBjb2xsaWRlciBtZXNoLlxuICAgKlxuICAgKiBAcGFyYW0gcmFkaXVzIFJhZGl1cyBvZiB0aGUgbmV3IGNvbGxpZGVyIG1lc2hcbiAgICogQHBhcmFtIG9mZnNldCBPZmZlc3Qgb2YgdGhlIG5ldyBjb2xsaWRlciBtZXNoXG4gICAqL1xuICBwcm90ZWN0ZWQgX2NyZWF0ZUNvbGxpZGVyTWVzaChyYWRpdXM6IG51bWJlciwgb2Zmc2V0OiBUSFJFRS5WZWN0b3IzKTogVlJNU3ByaW5nQm9uZUNvbGxpZGVyTWVzaCB7XG4gICAgY29uc3QgY29sbGlkZXJNZXNoID0gbmV3IFRIUkVFLk1lc2gobmV3IFRIUkVFLlNwaGVyZUJ1ZmZlckdlb21ldHJ5KHJhZGl1cywgOCwgNCksIF9jb2xsaWRlck1hdGVyaWFsKTtcblxuICAgIGNvbGxpZGVyTWVzaC5wb3NpdGlvbi5jb3B5KG9mZnNldCk7XG5cbiAgICAvLyB0aGUgbmFtZSBoYXZlIHRvIGJlIHRoaXMgaW4gb3JkZXIgdG8gZXhjbHVkZSBjb2xsaWRlcnMgZnJvbSBib3VuZGluZyBib3hcbiAgICAvLyAoU2VlIFZpZXdlci50cywgc2VhcmNoIGZvciBjaGlsZC5uYW1lID09PSAndnJtQ29sbGlkZXJTcGhlcmUnKVxuICAgIGNvbGxpZGVyTWVzaC5uYW1lID0gJ3ZybUNvbGxpZGVyU3BoZXJlJztcblxuICAgIC8vIFdlIHdpbGwgdXNlIHRoZSByYWRpdXMgb2YgdGhlIHNwaGVyZSBmb3IgY29sbGlzaW9uIHZzIGJvbmVzLlxuICAgIC8vIGBib3VuZGluZ1NwaGVyZWAgbXVzdCBiZSBjcmVhdGVkIHRvIGNvbXB1dGUgdGhlIHJhZGl1cy5cbiAgICBjb2xsaWRlck1lc2guZ2VvbWV0cnkuY29tcHV0ZUJvdW5kaW5nU3BoZXJlKCk7XG5cbiAgICByZXR1cm4gY29sbGlkZXJNZXNoO1xuICB9XG59XG4iLCJpbXBvcnQgeyBHTFRGIH0gZnJvbSAndGhyZWUvZXhhbXBsZXMvanNtL2xvYWRlcnMvR0xURkxvYWRlcic7XG5pbXBvcnQgeyBWUk1CbGVuZFNoYXBlSW1wb3J0ZXIgfSBmcm9tICcuL2JsZW5kc2hhcGUnO1xuaW1wb3J0IHsgVlJNRmlyc3RQZXJzb25JbXBvcnRlciB9IGZyb20gJy4vZmlyc3RwZXJzb24nO1xuaW1wb3J0IHsgVlJNSHVtYW5vaWRJbXBvcnRlciB9IGZyb20gJy4vaHVtYW5vaWQvVlJNSHVtYW5vaWRJbXBvcnRlcic7XG5pbXBvcnQgeyBWUk1Mb29rQXRJbXBvcnRlciB9IGZyb20gJy4vbG9va2F0L1ZSTUxvb2tBdEltcG9ydGVyJztcbmltcG9ydCB7IFZSTU1hdGVyaWFsSW1wb3J0ZXIgfSBmcm9tICcuL21hdGVyaWFsJztcbmltcG9ydCB7IFZSTU1ldGFJbXBvcnRlciB9IGZyb20gJy4vbWV0YS9WUk1NZXRhSW1wb3J0ZXInO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZUltcG9ydGVyIH0gZnJvbSAnLi9zcHJpbmdib25lL1ZSTVNwcmluZ0JvbmVJbXBvcnRlcic7XG5pbXBvcnQgeyBWUk0gfSBmcm9tICcuL1ZSTSc7XG5cbmV4cG9ydCBpbnRlcmZhY2UgVlJNSW1wb3J0ZXJPcHRpb25zIHtcbiAgbWV0YUltcG9ydGVyPzogVlJNTWV0YUltcG9ydGVyO1xuICBsb29rQXRJbXBvcnRlcj86IFZSTUxvb2tBdEltcG9ydGVyO1xuICBodW1hbm9pZEltcG9ydGVyPzogVlJNSHVtYW5vaWRJbXBvcnRlcjtcbiAgYmxlbmRTaGFwZUltcG9ydGVyPzogVlJNQmxlbmRTaGFwZUltcG9ydGVyO1xuICBmaXJzdFBlcnNvbkltcG9ydGVyPzogVlJNRmlyc3RQZXJzb25JbXBvcnRlcjtcbiAgbWF0ZXJpYWxJbXBvcnRlcj86IFZSTU1hdGVyaWFsSW1wb3J0ZXI7XG4gIHNwcmluZ0JvbmVJbXBvcnRlcj86IFZSTVNwcmluZ0JvbmVJbXBvcnRlcjtcbn1cblxuLyoqXG4gKiBBbiBpbXBvcnRlciB0aGF0IGltcG9ydHMgYSBbW1ZSTV1dIGZyb20gYSBWUk0gZXh0ZW5zaW9uIG9mIGEgR0xURi5cbiAqL1xuZXhwb3J0IGNsYXNzIFZSTUltcG9ydGVyIHtcbiAgcHJvdGVjdGVkIHJlYWRvbmx5IF9tZXRhSW1wb3J0ZXI6IFZSTU1ldGFJbXBvcnRlcjtcbiAgcHJvdGVjdGVkIHJlYWRvbmx5IF9ibGVuZFNoYXBlSW1wb3J0ZXI6IFZSTUJsZW5kU2hhcGVJbXBvcnRlcjtcbiAgcHJvdGVjdGVkIHJlYWRvbmx5IF9sb29rQXRJbXBvcnRlcjogVlJNTG9va0F0SW1wb3J0ZXI7XG4gIHByb3RlY3RlZCByZWFkb25seSBfaHVtYW5vaWRJbXBvcnRlcjogVlJNSHVtYW5vaWRJbXBvcnRlcjtcbiAgcHJvdGVjdGVkIHJlYWRvbmx5IF9maXJzdFBlcnNvbkltcG9ydGVyOiBWUk1GaXJzdFBlcnNvbkltcG9ydGVyO1xuICBwcm90ZWN0ZWQgcmVhZG9ubHkgX21hdGVyaWFsSW1wb3J0ZXI6IFZSTU1hdGVyaWFsSW1wb3J0ZXI7XG4gIHByb3RlY3RlZCByZWFkb25seSBfc3ByaW5nQm9uZUltcG9ydGVyOiBWUk1TcHJpbmdCb25lSW1wb3J0ZXI7XG5cbiAgLyoqXG4gICAqIENyZWF0ZSBhIG5ldyBWUk1JbXBvcnRlci5cbiAgICpcbiAgICogQHBhcmFtIG9wdGlvbnMgW1tWUk1JbXBvcnRlck9wdGlvbnNdXSwgb3B0aW9uYWxseSBjb250YWlucyBpbXBvcnRlcnMgZm9yIGVhY2ggY29tcG9uZW50XG4gICAqL1xuICBwdWJsaWMgY29uc3RydWN0b3Iob3B0aW9uczogVlJNSW1wb3J0ZXJPcHRpb25zID0ge30pIHtcbiAgICB0aGlzLl9tZXRhSW1wb3J0ZXIgPSBvcHRpb25zLm1ldGFJbXBvcnRlciB8fCBuZXcgVlJNTWV0YUltcG9ydGVyKCk7XG4gICAgdGhpcy5fYmxlbmRTaGFwZUltcG9ydGVyID0gb3B0aW9ucy5ibGVuZFNoYXBlSW1wb3J0ZXIgfHwgbmV3IFZSTUJsZW5kU2hhcGVJbXBvcnRlcigpO1xuICAgIHRoaXMuX2xvb2tBdEltcG9ydGVyID0gb3B0aW9ucy5sb29rQXRJbXBvcnRlciB8fCBuZXcgVlJNTG9va0F0SW1wb3J0ZXIoKTtcbiAgICB0aGlzLl9odW1hbm9pZEltcG9ydGVyID0gb3B0aW9ucy5odW1hbm9pZEltcG9ydGVyIHx8IG5ldyBWUk1IdW1hbm9pZEltcG9ydGVyKCk7XG4gICAgdGhpcy5fZmlyc3RQZXJzb25JbXBvcnRlciA9IG9wdGlvbnMuZmlyc3RQZXJzb25JbXBvcnRlciB8fCBuZXcgVlJNRmlyc3RQZXJzb25JbXBvcnRlcigpO1xuICAgIHRoaXMuX21hdGVyaWFsSW1wb3J0ZXIgPSBvcHRpb25zLm1hdGVyaWFsSW1wb3J0ZXIgfHwgbmV3IFZSTU1hdGVyaWFsSW1wb3J0ZXIoKTtcbiAgICB0aGlzLl9zcHJpbmdCb25lSW1wb3J0ZXIgPSBvcHRpb25zLnNwcmluZ0JvbmVJbXBvcnRlciB8fCBuZXcgVlJNU3ByaW5nQm9uZUltcG9ydGVyKCk7XG4gIH1cblxuICAvKipcbiAgICogUmVjZWl2ZSBhIEdMVEYgb2JqZWN0IHJldHJpZXZlZCBmcm9tIGBUSFJFRS5HTFRGTG9hZGVyYCBhbmQgY3JlYXRlIGEgbmV3IFtbVlJNXV0gaW5zdGFuY2UuXG4gICAqXG4gICAqIEBwYXJhbSBnbHRmIEEgcGFyc2VkIHJlc3VsdCBvZiBHTFRGIHRha2VuIGZyb20gR0xURkxvYWRlclxuICAgKi9cbiAgcHVibGljIGFzeW5jIGltcG9ydChnbHRmOiBHTFRGKTogUHJvbWlzZTxWUk0+IHtcbiAgICBpZiAoZ2x0Zi5wYXJzZXIuanNvbi5leHRlbnNpb25zID09PSB1bmRlZmluZWQgfHwgZ2x0Zi5wYXJzZXIuanNvbi5leHRlbnNpb25zLlZSTSA9PT0gdW5kZWZpbmVkKSB7XG4gICAgICB0aHJvdyBuZXcgRXJyb3IoJ0NvdWxkIG5vdCBmaW5kIFZSTSBleHRlbnNpb24gb24gdGhlIEdMVEYnKTtcbiAgICB9XG4gICAgY29uc3Qgc2NlbmUgPSBnbHRmLnNjZW5lO1xuXG4gICAgc2NlbmUudXBkYXRlTWF0cml4V29ybGQoZmFsc2UpO1xuXG4gICAgLy8gU2tpbm5lZCBvYmplY3Qgc2hvdWxkIG5vdCBiZSBmcnVzdHVtQ3VsbGVkXG4gICAgLy8gU2luY2UgcHJlLXNraW5uZWQgcG9zaXRpb24gbWlnaHQgYmUgb3V0c2lkZSBvZiB2aWV3XG4gICAgc2NlbmUudHJhdmVyc2UoKG9iamVjdDNkKSA9PiB7XG4gICAgICBpZiAoKG9iamVjdDNkIGFzIGFueSkuaXNNZXNoKSB7XG4gICAgICAgIG9iamVjdDNkLmZydXN0dW1DdWxsZWQgPSBmYWxzZTtcbiAgICAgIH1cbiAgICB9KTtcblxuICAgIGNvbnN0IG1ldGEgPSAoYXdhaXQgdGhpcy5fbWV0YUltcG9ydGVyLmltcG9ydChnbHRmKSkgfHwgdW5kZWZpbmVkO1xuXG4gICAgY29uc3QgbWF0ZXJpYWxzID0gKGF3YWl0IHRoaXMuX21hdGVyaWFsSW1wb3J0ZXIuY29udmVydEdMVEZNYXRlcmlhbHMoZ2x0ZikpIHx8IHVuZGVmaW5lZDtcblxuICAgIGNvbnN0IGh1bWFub2lkID0gKGF3YWl0IHRoaXMuX2h1bWFub2lkSW1wb3J0ZXIuaW1wb3J0KGdsdGYpKSB8fCB1bmRlZmluZWQ7XG5cbiAgICBjb25zdCBmaXJzdFBlcnNvbiA9IGh1bWFub2lkID8gKGF3YWl0IHRoaXMuX2ZpcnN0UGVyc29uSW1wb3J0ZXIuaW1wb3J0KGdsdGYsIGh1bWFub2lkKSkgfHwgdW5kZWZpbmVkIDogdW5kZWZpbmVkO1xuXG4gICAgY29uc3QgYmxlbmRTaGFwZVByb3h5ID0gKGF3YWl0IHRoaXMuX2JsZW5kU2hhcGVJbXBvcnRlci5pbXBvcnQoZ2x0ZikpIHx8IHVuZGVmaW5lZDtcblxuICAgIGNvbnN0IGxvb2tBdCA9XG4gICAgICBmaXJzdFBlcnNvbiAmJiBibGVuZFNoYXBlUHJveHkgJiYgaHVtYW5vaWRcbiAgICAgICAgPyAoYXdhaXQgdGhpcy5fbG9va0F0SW1wb3J0ZXIuaW1wb3J0KGdsdGYsIGZpcnN0UGVyc29uLCBibGVuZFNoYXBlUHJveHksIGh1bWFub2lkKSkgfHwgdW5kZWZpbmVkXG4gICAgICAgIDogdW5kZWZpbmVkO1xuXG4gICAgY29uc3Qgc3ByaW5nQm9uZU1hbmFnZXIgPSAoYXdhaXQgdGhpcy5fc3ByaW5nQm9uZUltcG9ydGVyLmltcG9ydChnbHRmKSkgfHwgdW5kZWZpbmVkO1xuXG4gICAgcmV0dXJuIG5ldyBWUk0oe1xuICAgICAgc2NlbmU6IGdsdGYuc2NlbmUsXG4gICAgICBtZXRhLFxuICAgICAgbWF0ZXJpYWxzLFxuICAgICAgaHVtYW5vaWQsXG4gICAgICBmaXJzdFBlcnNvbixcbiAgICAgIGJsZW5kU2hhcGVQcm94eSxcbiAgICAgIGxvb2tBdCxcbiAgICAgIHNwcmluZ0JvbmVNYW5hZ2VyLFxuICAgIH0pO1xuICB9XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5pbXBvcnQgeyBHTFRGIH0gZnJvbSAndGhyZWUvZXhhbXBsZXMvanNtL2xvYWRlcnMvR0xURkxvYWRlcic7XG5pbXBvcnQgeyBWUk1CbGVuZFNoYXBlUHJveHkgfSBmcm9tICcuL2JsZW5kc2hhcGUnO1xuaW1wb3J0IHsgVlJNRmlyc3RQZXJzb24gfSBmcm9tICcuL2ZpcnN0cGVyc29uJztcbmltcG9ydCB7IFZSTUh1bWFub2lkIH0gZnJvbSAnLi9odW1hbm9pZCc7XG5pbXBvcnQgeyBWUk1Mb29rQXRIZWFkIH0gZnJvbSAnLi9sb29rYXQnO1xuaW1wb3J0IHsgVlJNTWV0YSB9IGZyb20gJy4vbWV0YS9WUk1NZXRhJztcbmltcG9ydCB7IFZSTVNwcmluZ0JvbmVNYW5hZ2VyIH0gZnJvbSAnLi9zcHJpbmdib25lJztcbmltcG9ydCB7IGRlZXBEaXNwb3NlIH0gZnJvbSAnLi91dGlscy9kaXNwb3Nlcic7XG5pbXBvcnQgeyBWUk1JbXBvcnRlciwgVlJNSW1wb3J0ZXJPcHRpb25zIH0gZnJvbSAnLi9WUk1JbXBvcnRlcic7XG5cbi8qKlxuICogUGFyYW1ldGVycyBmb3IgYSBbW1ZSTV1dIGNsYXNzLlxuICovXG5leHBvcnQgaW50ZXJmYWNlIFZSTVBhcmFtZXRlcnMge1xuICBzY2VuZTogVEhSRUUuU2NlbmUgfCBUSFJFRS5Hcm91cDsgLy8gQ09NUEFUOiBgR0xURi5zY2VuZWAgaXMgZ29pbmcgdG8gYmUgYFRIUkVFLkdyb3VwYCBpbiByMTE0XG4gIGh1bWFub2lkPzogVlJNSHVtYW5vaWQ7XG4gIGJsZW5kU2hhcGVQcm94eT86IFZSTUJsZW5kU2hhcGVQcm94eTtcbiAgZmlyc3RQZXJzb24/OiBWUk1GaXJzdFBlcnNvbjtcbiAgbG9va0F0PzogVlJNTG9va0F0SGVhZDtcbiAgbWF0ZXJpYWxzPzogVEhSRUUuTWF0ZXJpYWxbXTtcbiAgc3ByaW5nQm9uZU1hbmFnZXI/OiBWUk1TcHJpbmdCb25lTWFuYWdlcjtcbiAgbWV0YT86IFZSTU1ldGE7XG59XG5cbi8qKlxuICogQSBjbGFzcyB0aGF0IHJlcHJlc2VudHMgYSBzaW5nbGUgVlJNIG1vZGVsLlxuICogU2VlIHRoZSBkb2N1bWVudGF0aW9uIG9mIFtbVlJNLmZyb21dXSBmb3IgdGhlIG1vc3QgYmFzaWMgdXNlIG9mIFZSTS5cbiAqL1xuZXhwb3J0IGNsYXNzIFZSTSB7XG4gIC8qKlxuICAgKiBDcmVhdGUgYSBuZXcgVlJNIGZyb20gYSBwYXJzZWQgcmVzdWx0IG9mIEdMVEYgdGFrZW4gZnJvbSBHTFRGTG9hZGVyLlxuICAgKiBJdCdzIHByb2JhYmx5IGEgdGhpbmcgd2hhdCB5b3Ugd2FudCB0byBnZXQgc3RhcnRlZCB3aXRoIFZSTXMuXG4gICAqXG4gICAqIEBleGFtcGxlIE1vc3QgYmFzaWMgdXNlIG9mIFZSTVxuICAgKiBgYGBcbiAgICogY29uc3Qgc2NlbmUgPSBuZXcgVEhSRUUuU2NlbmUoKTtcbiAgICpcbiAgICogbmV3IFRIUkVFLkdMVEZMb2FkZXIoKS5sb2FkKCAnbW9kZWxzL3RocmVlLXZybS1naXJsLnZybScsICggZ2x0ZiApID0+IHtcbiAgICpcbiAgICogICBUSFJFRS5WUk0uZnJvbSggZ2x0ZiApLnRoZW4oICggdnJtICkgPT4ge1xuICAgKlxuICAgKiAgICAgc2NlbmUuYWRkKCB2cm0uc2NlbmUgKTtcbiAgICpcbiAgICogICB9ICk7XG4gICAqXG4gICAqIH0gKTtcbiAgICogYGBgXG4gICAqXG4gICAqIEBwYXJhbSBnbHRmIEEgcGFyc2VkIEdMVEYgb2JqZWN0IHRha2VuIGZyb20gR0xURkxvYWRlclxuICAgKiBAcGFyYW0gb3B0aW9ucyBPcHRpb25zIHRoYXQgd2lsbCBiZSB1c2VkIGluIGltcG9ydGVyXG4gICAqL1xuICBwdWJsaWMgc3RhdGljIGFzeW5jIGZyb20oZ2x0ZjogR0xURiwgb3B0aW9uczogVlJNSW1wb3J0ZXJPcHRpb25zID0ge30pOiBQcm9taXNlPFZSTT4ge1xuICAgIGNvbnN0IGltcG9ydGVyID0gbmV3IFZSTUltcG9ydGVyKG9wdGlvbnMpO1xuICAgIHJldHVybiBhd2FpdCBpbXBvcnRlci5pbXBvcnQoZ2x0Zik7XG4gIH1cbiAgLyoqXG4gICAqIGBUSFJFRS5TY2VuZWAgb3IgYFRIUkVFLkdyb3VwYCAoZGVwZW5kcyBvbiB5b3VyIHRocmVlLmpzIHJldmlzaW9uKSB0aGF0IGNvbnRhaW5zIHRoZSBlbnRpcmUgVlJNLlxuICAgKi9cbiAgcHVibGljIHJlYWRvbmx5IHNjZW5lOiBUSFJFRS5TY2VuZSB8IFRIUkVFLkdyb3VwOyAvLyBDT01QQVQ6IGBHTFRGLnNjZW5lYCBpcyBnb2luZyB0byBiZSBgVEhSRUUuR3JvdXBgIGluIHIxMTRcblxuICAvKipcbiAgICogQ29udGFpbnMgW1tWUk1IdW1hbm9pZF1dIG9mIHRoZSBWUk0uXG4gICAqIFlvdSBjYW4gY29udHJvbCBlYWNoIGJvbmVzIHVzaW5nIFtbVlJNSHVtYW5vaWQuZ2V0Qm9uZU5vZGVdXS5cbiAgICpcbiAgICogQFRPRE8gQWRkIGEgbGluayB0byBWUk0gc3BlY1xuICAgKi9cbiAgcHVibGljIHJlYWRvbmx5IGh1bWFub2lkPzogVlJNSHVtYW5vaWQ7XG5cbiAgLyoqXG4gICAqIENvbnRhaW5zIFtbVlJNQmxlbmRTaGFwZVByb3h5XV0gb2YgdGhlIFZSTS5cbiAgICogWW91IG1pZ2h0IHdhbnQgdG8gY29udHJvbCB0aGVzZSBmYWNpYWwgZXhwcmVzc2lvbnMgdmlhIFtbVlJNQmxlbmRTaGFwZVByb3h5LnNldFZhbHVlXV0uXG4gICAqL1xuICBwdWJsaWMgcmVhZG9ubHkgYmxlbmRTaGFwZVByb3h5PzogVlJNQmxlbmRTaGFwZVByb3h5O1xuXG4gIC8qKlxuICAgKiBDb250YWlucyBbW1ZSTUZpcnN0UGVyc29uXV0gb2YgdGhlIFZSTS5cbiAgICogWW91IGNhbiB1c2UgdmFyaW91cyBmZWF0dXJlIG9mIHRoZSBmaXJzdFBlcnNvbiBmaWVsZC5cbiAgICovXG4gIHB1YmxpYyByZWFkb25seSBmaXJzdFBlcnNvbj86IFZSTUZpcnN0UGVyc29uO1xuXG4gIC8qKlxuICAgKiBDb250YWlucyBbW1ZSTUxvb2tBdEhlYWRdXSBvZiB0aGUgVlJNLlxuICAgKiBZb3UgbWlnaHQgd2FudCB0byB1c2UgW1tWUk1Mb29rQXRIZWFkLnRhcmdldF1dIHRvIGNvbnRyb2wgdGhlIGV5ZSBkaXJlY3Rpb24gb2YgeW91ciBWUk1zLlxuICAgKi9cbiAgcHVibGljIHJlYWRvbmx5IGxvb2tBdD86IFZSTUxvb2tBdEhlYWQ7XG5cbiAgLyoqXG4gICAqIENvbnRhaW5zIG1hdGVyaWFscyBvZiB0aGUgVlJNLlxuICAgKiBgdXBkYXRlVlJNTWF0ZXJpYWxzYCBtZXRob2Qgb2YgdGhlc2UgbWF0ZXJpYWxzIHdpbGwgYmUgY2FsbGVkIHZpYSBpdHMgW1tWUk0udXBkYXRlXV0gbWV0aG9kLlxuICAgKi9cbiAgcHVibGljIHJlYWRvbmx5IG1hdGVyaWFscz86IFRIUkVFLk1hdGVyaWFsW107XG5cbiAgLyoqXG4gICAqIENvbnRhaW5zIG1ldGEgZmllbGRzIG9mIHRoZSBWUk0uXG4gICAqIFlvdSBtaWdodCB3YW50IHRvIHJlZmVyIHRoZXNlIGxpY2Vuc2UgZmllbGRzIGJlZm9yZSB1c2UgeW91ciBWUk1zLlxuICAgKi9cbiAgcHVibGljIHJlYWRvbmx5IG1ldGE/OiBWUk1NZXRhO1xuXG4gIC8qKlxuICAgKiBBIFtbVlJNU3ByaW5nQm9uZU1hbmFnZXJdXSBtYW5pcHVsYXRlcyBhbGwgc3ByaW5nIGJvbmVzIGF0dGFjaGVkIG9uIHRoZSBWUk0uXG4gICAqIFVzdWFsbHkgeW91IGRvbid0IGhhdmUgdG8gY2FyZSBhYm91dCB0aGlzIHByb3BlcnR5LlxuICAgKi9cbiAgcHVibGljIHJlYWRvbmx5IHNwcmluZ0JvbmVNYW5hZ2VyPzogVlJNU3ByaW5nQm9uZU1hbmFnZXI7XG5cbiAgLyoqXG4gICAqIENyZWF0ZSBhIG5ldyBWUk0gaW5zdGFuY2UuXG4gICAqXG4gICAqIEBwYXJhbSBwYXJhbXMgW1tWUk1QYXJhbWV0ZXJzXV0gdGhhdCByZXByZXNlbnRzIGNvbXBvbmVudHMgb2YgdGhlIFZSTVxuICAgKi9cbiAgcHVibGljIGNvbnN0cnVjdG9yKHBhcmFtczogVlJNUGFyYW1ldGVycykge1xuICAgIHRoaXMuc2NlbmUgPSBwYXJhbXMuc2NlbmU7XG4gICAgdGhpcy5odW1hbm9pZCA9IHBhcmFtcy5odW1hbm9pZDtcbiAgICB0aGlzLmJsZW5kU2hhcGVQcm94eSA9IHBhcmFtcy5ibGVuZFNoYXBlUHJveHk7XG4gICAgdGhpcy5maXJzdFBlcnNvbiA9IHBhcmFtcy5maXJzdFBlcnNvbjtcbiAgICB0aGlzLmxvb2tBdCA9IHBhcmFtcy5sb29rQXQ7XG4gICAgdGhpcy5tYXRlcmlhbHMgPSBwYXJhbXMubWF0ZXJpYWxzO1xuICAgIHRoaXMuc3ByaW5nQm9uZU1hbmFnZXIgPSBwYXJhbXMuc3ByaW5nQm9uZU1hbmFnZXI7XG4gICAgdGhpcy5tZXRhID0gcGFyYW1zLm1ldGE7XG4gIH1cblxuICAvKipcbiAgICogKipZb3UgbmVlZCB0byBjYWxsIHRoaXMgb24geW91ciB1cGRhdGUgbG9vcC4qKlxuICAgKlxuICAgKiBUaGlzIGZ1bmN0aW9uIHVwZGF0ZXMgZXZlcnkgVlJNIGNvbXBvbmVudHMuXG4gICAqXG4gICAqIEBwYXJhbSBkZWx0YSBkZWx0YVRpbWVcbiAgICovXG4gIHB1YmxpYyB1cGRhdGUoZGVsdGE6IG51bWJlcik6IHZvaWQge1xuICAgIGlmICh0aGlzLmxvb2tBdCkge1xuICAgICAgdGhpcy5sb29rQXQudXBkYXRlKGRlbHRhKTtcbiAgICB9XG5cbiAgICBpZiAodGhpcy5ibGVuZFNoYXBlUHJveHkpIHtcbiAgICAgIHRoaXMuYmxlbmRTaGFwZVByb3h5LnVwZGF0ZSgpO1xuICAgIH1cblxuICAgIGlmICh0aGlzLnNwcmluZ0JvbmVNYW5hZ2VyKSB7XG4gICAgICB0aGlzLnNwcmluZ0JvbmVNYW5hZ2VyLmxhdGVVcGRhdGUoZGVsdGEpO1xuICAgIH1cblxuICAgIGlmICh0aGlzLm1hdGVyaWFscykge1xuICAgICAgdGhpcy5tYXRlcmlhbHMuZm9yRWFjaCgobWF0ZXJpYWw6IGFueSkgPT4ge1xuICAgICAgICBpZiAobWF0ZXJpYWwudXBkYXRlVlJNTWF0ZXJpYWxzKSB7XG4gICAgICAgICAgbWF0ZXJpYWwudXBkYXRlVlJNTWF0ZXJpYWxzKGRlbHRhKTtcbiAgICAgICAgfVxuICAgICAgfSk7XG4gICAgfVxuICB9XG5cbiAgLyoqXG4gICAqIERpc3Bvc2UgZXZlcnl0aGluZyBhYm91dCB0aGUgVlJNIGluc3RhbmNlLlxuICAgKi9cbiAgcHVibGljIGRpc3Bvc2UoKTogdm9pZCB7XG4gICAgY29uc3Qgc2NlbmUgPSB0aGlzLnNjZW5lO1xuICAgIGlmIChzY2VuZSkge1xuICAgICAgZGVlcERpc3Bvc2Uoc2NlbmUpO1xuICAgIH1cblxuICAgIHRoaXMubWV0YT8udGV4dHVyZT8uZGlzcG9zZSgpO1xuICB9XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5pbXBvcnQgeyBWUk0gfSBmcm9tICcuLi9WUk0nO1xuXG5jb25zdCBfdjJBID0gbmV3IFRIUkVFLlZlY3RvcjIoKTtcblxuY29uc3QgX2NhbWVyYSA9IG5ldyBUSFJFRS5PcnRob2dyYXBoaWNDYW1lcmEoLTEsIDEsIC0xLCAxLCAtMSwgMSk7XG5jb25zdCBfbWF0ZXJpYWwgPSBuZXcgVEhSRUUuTWVzaEJhc2ljTWF0ZXJpYWwoeyBjb2xvcjogMHhmZmZmZmYsIHNpZGU6IFRIUkVFLkRvdWJsZVNpZGUgfSk7XG5jb25zdCBfcGxhbmUgPSBuZXcgVEhSRUUuTWVzaChuZXcgVEhSRUUuUGxhbmVCdWZmZXJHZW9tZXRyeSgyLCAyKSwgX21hdGVyaWFsKTtcbmNvbnN0IF9zY2VuZSA9IG5ldyBUSFJFRS5TY2VuZSgpO1xuX3NjZW5lLmFkZChfcGxhbmUpO1xuXG4vKipcbiAqIEV4dHJhY3QgYSB0aHVtYm5haWwgaW1hZ2UgYmxvYiBmcm9tIGEge0BsaW5rIFZSTX0uXG4gKiBJZiB0aGUgdnJtIGRvZXMgbm90IGhhdmUgYSB0aHVtYm5haWwsIGl0IHdpbGwgdGhyb3cgYW4gZXJyb3IuXG4gKiBAcGFyYW0gcmVuZGVyZXIgUmVuZGVyZXJcbiAqIEBwYXJhbSB2cm0gVlJNIHdpdGggYSB0aHVtYm5haWxcbiAqIEBwYXJhbSBzaXplIHdpZHRoIC8gaGVpZ2h0IG9mIHRoZSBpbWFnZVxuICovXG5leHBvcnQgZnVuY3Rpb24gZXh0cmFjdFRodW1ibmFpbEJsb2IocmVuZGVyZXI6IFRIUkVFLldlYkdMUmVuZGVyZXIsIHZybTogVlJNLCBzaXplID0gNTEyKTogUHJvbWlzZTxCbG9iPiB7XG4gIC8vIGdldCB0aGUgdGV4dHVyZVxuICBjb25zdCB0ZXh0dXJlID0gdnJtLm1ldGE/LnRleHR1cmU7XG4gIGlmICghdGV4dHVyZSkge1xuICAgIHRocm93IG5ldyBFcnJvcignZXh0cmFjdFRodW1ibmFpbEJsb2I6IFRoaXMgVlJNIGRvZXMgbm90IGhhdmUgYSB0aHVtYm5haWwnKTtcbiAgfVxuXG4gIGNvbnN0IGNhbnZhcyA9IHJlbmRlcmVyLmdldENvbnRleHQoKS5jYW52YXM7XG5cbiAgLy8gc3RvcmUgdGhlIGN1cnJlbnQgcmVzb2x1dGlvblxuICByZW5kZXJlci5nZXRTaXplKF92MkEpO1xuICBjb25zdCBwcmV2V2lkdGggPSBfdjJBLng7XG4gIGNvbnN0IHByZXZIZWlnaHQgPSBfdjJBLnk7XG5cbiAgLy8gb3ZlcndyaXRlIHRoZSByZXNvbHV0aW9uXG4gIHJlbmRlcmVyLnNldFNpemUoc2l6ZSwgc2l6ZSwgZmFsc2UpO1xuXG4gIC8vIGFzc2lnbiB0aGUgdGV4dHVyZSB0byBwbGFuZVxuICBfbWF0ZXJpYWwubWFwID0gdGV4dHVyZTtcblxuICAvLyByZW5kZXJcbiAgcmVuZGVyZXIucmVuZGVyKF9zY2VuZSwgX2NhbWVyYSk7XG5cbiAgLy8gdW5hc3NpZ24gdGhlIHRleHR1cmVcbiAgX21hdGVyaWFsLm1hcCA9IG51bGw7XG5cbiAgLy8gZ2V0IGJsb2JcbiAgaWYgKGNhbnZhcyBpbnN0YW5jZW9mIE9mZnNjcmVlbkNhbnZhcykge1xuICAgIHJldHVybiBjYW52YXMuY29udmVydFRvQmxvYigpLmZpbmFsbHkoKCkgPT4ge1xuICAgICAgLy8gcmV2ZXJ0IHRvIHByZXZpb3VzIHJlc29sdXRpb25cbiAgICAgIHJlbmRlcmVyLnNldFNpemUocHJldldpZHRoLCBwcmV2SGVpZ2h0LCBmYWxzZSk7XG4gICAgfSk7XG4gIH1cblxuICByZXR1cm4gbmV3IFByb21pc2UoKHJlc29sdmUsIHJlamVjdCkgPT4ge1xuICAgIGNhbnZhcy50b0Jsb2IoKGJsb2IpID0+IHtcbiAgICAgIC8vIHJldmVydCB0byBwcmV2aW91cyByZXNvbHV0aW9uXG4gICAgICByZW5kZXJlci5zZXRTaXplKHByZXZXaWR0aCwgcHJldkhlaWdodCwgZmFsc2UpO1xuXG4gICAgICBpZiAoYmxvYiA9PSBudWxsKSB7XG4gICAgICAgIHJlamVjdCgnZXh0cmFjdFRodW1ibmFpbEJsb2I6IEZhaWxlZCB0byBjcmVhdGUgYSBibG9iJyk7XG4gICAgICB9IGVsc2Uge1xuICAgICAgICByZXNvbHZlKGJsb2IpO1xuICAgICAgfVxuICAgIH0pO1xuICB9KTtcbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcblxuLyoqXG4gKiBUcmF2ZXJzZSBnaXZlbiBvYmplY3QgYW5kIHJlbW92ZSB1bm5lY2Vzc2FyaWx5IGJvdW5kIGpvaW50cyBmcm9tIGV2ZXJ5IGBUSFJFRS5Ta2lubmVkTWVzaGAuXG4gKiBTb21lIGVudmlyb25tZW50cyBsaWtlIG1vYmlsZSBkZXZpY2VzIGhhdmUgYSBsb3dlciBsaW1pdCBvZiBib25lcyBhbmQgbWlnaHQgYmUgdW5hYmxlIHRvIHBlcmZvcm0gbWVzaCBza2lubmluZywgdGhpcyBmdW5jdGlvbiBtaWdodCByZXNvbHZlIHN1Y2ggYW4gaXNzdWUuXG4gKiBBbHNvIHRoaXMgZnVuY3Rpb24gbWlnaHQgZ3JlYXRseSBpbXByb3ZlIHRoZSBwZXJmb3JtYW5jZSBvZiBtZXNoIHNraW5uaW5nLlxuICpcbiAqIEBwYXJhbSByb290IFJvb3Qgb2JqZWN0IHRoYXQgd2lsbCBiZSB0cmF2ZXJzZWRcbiAqL1xuZXhwb3J0IGZ1bmN0aW9uIHJlbW92ZVVubmVjZXNzYXJ5Sm9pbnRzKHJvb3Q6IFRIUkVFLk9iamVjdDNEKTogdm9pZCB7XG4gIC8vIHNvbWUgbWVzaGVzIG1pZ2h0IHNoYXJlIGEgc2FtZSBza2luSW5kZXggYXR0cmlidXRlIGFuZCB0aGlzIG1hcCBwcmV2ZW50cyB0byBjb252ZXJ0IHRoZSBhdHRyaWJ1dGUgdHdpY2VcbiAgY29uc3Qgc2tlbGV0b25MaXN0OiBNYXA8VEhSRUUuQnVmZmVyQXR0cmlidXRlLCBUSFJFRS5Ta2VsZXRvbj4gPSBuZXcgTWFwKCk7XG5cbiAgLy8gVHJhdmVyc2UgYW4gZW50aXJlIHRyZWVcbiAgcm9vdC50cmF2ZXJzZSgob2JqKSA9PiB7XG4gICAgaWYgKG9iai50eXBlICE9PSAnU2tpbm5lZE1lc2gnKSB7XG4gICAgICByZXR1cm47XG4gICAgfVxuXG4gICAgY29uc3QgbWVzaCA9IG9iaiBhcyBUSFJFRS5Ta2lubmVkTWVzaDtcbiAgICBjb25zdCBnZW9tZXRyeSA9IG1lc2guZ2VvbWV0cnkgYXMgVEhSRUUuQnVmZmVyR2VvbWV0cnk7XG4gICAgY29uc3QgYXR0cmlidXRlID0gZ2VvbWV0cnkuZ2V0QXR0cmlidXRlKCdza2luSW5kZXgnKSBhcyBUSFJFRS5CdWZmZXJBdHRyaWJ1dGU7XG5cbiAgICAvLyBsb29rIGZvciBleGlzdGluZyBza2VsZXRvblxuICAgIGxldCBza2VsZXRvbiA9IHNrZWxldG9uTGlzdC5nZXQoYXR0cmlidXRlKTtcblxuICAgIGlmICghc2tlbGV0b24pIHtcbiAgICAgIC8vIGdlbmVyYXRlIHJlZHVjZWQgYm9uZSBsaXN0XG4gICAgICBjb25zdCBib25lczogVEhSRUUuQm9uZVtdID0gW107IC8vIG5ldyBsaXN0IG9mIGJvbmVcbiAgICAgIGNvbnN0IGJvbmVJbnZlcnNlczogVEhSRUUuTWF0cml4NFtdID0gW107IC8vIG5ldyBsaXN0IG9mIGJvbmVJbnZlcnNlXG4gICAgICBjb25zdCBib25lSW5kZXhNYXA6IHsgW2luZGV4OiBudW1iZXJdOiBudW1iZXIgfSA9IHt9OyAvLyBtYXAgb2Ygb2xkIGJvbmUgaW5kZXggdnMuIG5ldyBib25lIGluZGV4XG5cbiAgICAgIC8vIGNyZWF0ZSBhIG5ldyBib25lIG1hcFxuICAgICAgY29uc3QgYXJyYXkgPSBhdHRyaWJ1dGUuYXJyYXkgYXMgbnVtYmVyW107XG4gICAgICBmb3IgKGxldCBpID0gMDsgaSA8IGFycmF5Lmxlbmd0aDsgaSsrKSB7XG4gICAgICAgIGNvbnN0IGluZGV4ID0gYXJyYXlbaV07XG5cbiAgICAgICAgLy8gbmV3IHNraW5JbmRleCBidWZmZXJcbiAgICAgICAgaWYgKGJvbmVJbmRleE1hcFtpbmRleF0gPT09IHVuZGVmaW5lZCkge1xuICAgICAgICAgIGJvbmVJbmRleE1hcFtpbmRleF0gPSBib25lcy5sZW5ndGg7XG4gICAgICAgICAgYm9uZXMucHVzaChtZXNoLnNrZWxldG9uLmJvbmVzW2luZGV4XSk7XG4gICAgICAgICAgYm9uZUludmVyc2VzLnB1c2gobWVzaC5za2VsZXRvbi5ib25lSW52ZXJzZXNbaW5kZXhdKTtcbiAgICAgICAgfVxuXG4gICAgICAgIGFycmF5W2ldID0gYm9uZUluZGV4TWFwW2luZGV4XTtcbiAgICAgIH1cblxuICAgICAgLy8gcmVwbGFjZSB3aXRoIG5ldyBpbmRpY2VzXG4gICAgICBhdHRyaWJ1dGUuY29weUFycmF5KGFycmF5KTtcbiAgICAgIGF0dHJpYnV0ZS5uZWVkc1VwZGF0ZSA9IHRydWU7XG5cbiAgICAgIC8vIHJlcGxhY2Ugd2l0aCBuZXcgaW5kaWNlc1xuICAgICAgc2tlbGV0b24gPSBuZXcgVEhSRUUuU2tlbGV0b24oYm9uZXMsIGJvbmVJbnZlcnNlcyk7XG4gICAgICBza2VsZXRvbkxpc3Quc2V0KGF0dHJpYnV0ZSwgc2tlbGV0b24pO1xuICAgIH1cblxuICAgIG1lc2guYmluZChza2VsZXRvbiwgbmV3IFRIUkVFLk1hdHJpeDQoKSk7XG4gICAgLy8gICAgICAgICAgICAgICAgICBeXl5eXl5eXl5eXl5eXl5eXl5eIHRyYW5zZm9ybSBvZiBtZXNoZXMgc2hvdWxkIGJlIGlnbm9yZWRcbiAgICAvLyBTZWU6IGh0dHBzOi8vZ2l0aHViLmNvbS9LaHJvbm9zR3JvdXAvZ2xURi90cmVlL21hc3Rlci9zcGVjaWZpY2F0aW9uLzIuMCNza2luc1xuICB9KTtcbn1cbiIsImltcG9ydCB7IGV4dHJhY3RUaHVtYm5haWxCbG9iIH0gZnJvbSAnLi9leHRyYWN0VGh1bWJuYWlsQmxvYic7XG5pbXBvcnQgeyByZW1vdmVVbm5lY2Vzc2FyeUpvaW50cyB9IGZyb20gJy4vcmVtb3ZlVW5uZWNlc3NhcnlKb2ludHMnO1xuXG5leHBvcnQgY2xhc3MgVlJNVXRpbHMge1xuICBwcml2YXRlIGNvbnN0cnVjdG9yKCkge1xuICAgIC8vIHRoaXMgY2xhc3MgaXMgbm90IG1lYW50IHRvIGJlIGluc3RhbnRpYXRlZFxuICB9XG5cbiAgcHVibGljIHN0YXRpYyBleHRyYWN0VGh1bWJuYWlsQmxvYiA9IGV4dHJhY3RUaHVtYm5haWxCbG9iO1xuICBwdWJsaWMgc3RhdGljIHJlbW92ZVVubmVjZXNzYXJ5Sm9pbnRzID0gcmVtb3ZlVW5uZWNlc3NhcnlKb2ludHM7XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5pbXBvcnQgeyBWUk1Mb29rQXRIZWFkIH0gZnJvbSAnLi4vbG9va2F0L1ZSTUxvb2tBdEhlYWQnO1xuaW1wb3J0IHsgVlJNRGVidWdPcHRpb25zIH0gZnJvbSAnLi9WUk1EZWJ1Z09wdGlvbnMnO1xuXG5jb25zdCBfdjMgPSBuZXcgVEhSRUUuVmVjdG9yMygpO1xuXG5leHBvcnQgY2xhc3MgVlJNTG9va0F0SGVhZERlYnVnIGV4dGVuZHMgVlJNTG9va0F0SGVhZCB7XG4gIHByaXZhdGUgX2ZhY2VEaXJlY3Rpb25IZWxwZXI/OiBUSFJFRS5BcnJvd0hlbHBlcjtcblxuICBwdWJsaWMgc2V0dXBIZWxwZXIoc2NlbmU6IFRIUkVFLk9iamVjdDNELCBkZWJ1Z09wdGlvbjogVlJNRGVidWdPcHRpb25zKTogdm9pZCB7XG4gICAgaWYgKCFkZWJ1Z09wdGlvbi5kaXNhYmxlRmFjZURpcmVjdGlvbkhlbHBlcikge1xuICAgICAgdGhpcy5fZmFjZURpcmVjdGlvbkhlbHBlciA9IG5ldyBUSFJFRS5BcnJvd0hlbHBlcihcbiAgICAgICAgbmV3IFRIUkVFLlZlY3RvcjMoMCwgMCwgLTEpLFxuICAgICAgICBuZXcgVEhSRUUuVmVjdG9yMygwLCAwLCAwKSxcbiAgICAgICAgMC41LFxuICAgICAgICAweGZmMDBmZixcbiAgICAgICk7XG4gICAgICBzY2VuZS5hZGQodGhpcy5fZmFjZURpcmVjdGlvbkhlbHBlcik7XG4gICAgfVxuICB9XG5cbiAgcHVibGljIHVwZGF0ZShkZWx0YTogbnVtYmVyKTogdm9pZCB7XG4gICAgc3VwZXIudXBkYXRlKGRlbHRhKTtcblxuICAgIGlmICh0aGlzLl9mYWNlRGlyZWN0aW9uSGVscGVyKSB7XG4gICAgICB0aGlzLmZpcnN0UGVyc29uLmdldEZpcnN0UGVyc29uV29ybGRQb3NpdGlvbih0aGlzLl9mYWNlRGlyZWN0aW9uSGVscGVyLnBvc2l0aW9uKTtcbiAgICAgIHRoaXMuX2ZhY2VEaXJlY3Rpb25IZWxwZXIuc2V0RGlyZWN0aW9uKHRoaXMuZ2V0TG9va0F0V29ybGREaXJlY3Rpb24oX3YzKSk7XG4gICAgfVxuICB9XG59XG4iLCJpbXBvcnQgeyBHTFRGIH0gZnJvbSAndGhyZWUvZXhhbXBsZXMvanNtL2xvYWRlcnMvR0xURkxvYWRlcic7XG5pbXBvcnQgeyBWUk1CbGVuZFNoYXBlUHJveHkgfSBmcm9tICcuLi9ibGVuZHNoYXBlJztcbmltcG9ydCB7IFZSTUZpcnN0UGVyc29uIH0gZnJvbSAnLi4vZmlyc3RwZXJzb24nO1xuaW1wb3J0IHsgVlJNSHVtYW5vaWQgfSBmcm9tICcuLi9odW1hbm9pZCc7XG5pbXBvcnQgeyBWUk1Mb29rQXRIZWFkIH0gZnJvbSAnLi4vbG9va2F0L1ZSTUxvb2tBdEhlYWQnO1xuaW1wb3J0IHsgVlJNTG9va0F0SW1wb3J0ZXIgfSBmcm9tICcuLi9sb29rYXQvVlJNTG9va0F0SW1wb3J0ZXInO1xuaW1wb3J0IHsgVlJNU2NoZW1hIH0gZnJvbSAnLi4vdHlwZXMnO1xuaW1wb3J0IHsgVlJNTG9va0F0SGVhZERlYnVnIH0gZnJvbSAnLi9WUk1Mb29rQXRIZWFkRGVidWcnO1xuXG5leHBvcnQgY2xhc3MgVlJNTG9va0F0SW1wb3J0ZXJEZWJ1ZyBleHRlbmRzIFZSTUxvb2tBdEltcG9ydGVyIHtcbiAgcHVibGljIGltcG9ydChcbiAgICBnbHRmOiBHTFRGLFxuICAgIGZpcnN0UGVyc29uOiBWUk1GaXJzdFBlcnNvbixcbiAgICBibGVuZFNoYXBlUHJveHk6IFZSTUJsZW5kU2hhcGVQcm94eSxcbiAgICBodW1hbm9pZDogVlJNSHVtYW5vaWQsXG4gICk6IFZSTUxvb2tBdEhlYWQgfCBudWxsIHtcbiAgICBjb25zdCB2cm1FeHQ6IFZSTVNjaGVtYS5WUk0gfCB1bmRlZmluZWQgPSBnbHRmLnBhcnNlci5qc29uLmV4dGVuc2lvbnM/LlZSTTtcbiAgICBpZiAoIXZybUV4dCkge1xuICAgICAgcmV0dXJuIG51bGw7XG4gICAgfVxuXG4gICAgY29uc3Qgc2NoZW1hRmlyc3RQZXJzb246IFZSTVNjaGVtYS5GaXJzdFBlcnNvbiB8IHVuZGVmaW5lZCA9IHZybUV4dC5maXJzdFBlcnNvbjtcbiAgICBpZiAoIXNjaGVtYUZpcnN0UGVyc29uKSB7XG4gICAgICByZXR1cm4gbnVsbDtcbiAgICB9XG5cbiAgICBjb25zdCBhcHBseWVyID0gdGhpcy5faW1wb3J0QXBwbHllcihzY2hlbWFGaXJzdFBlcnNvbiwgYmxlbmRTaGFwZVByb3h5LCBodW1hbm9pZCk7XG4gICAgcmV0dXJuIG5ldyBWUk1Mb29rQXRIZWFkRGVidWcoZmlyc3RQZXJzb24sIGFwcGx5ZXIgfHwgdW5kZWZpbmVkKTtcbiAgfVxufVxuIiwiaW1wb3J0ICogYXMgVEhSRUUgZnJvbSAndGhyZWUnO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZU1hbmFnZXIgfSBmcm9tICcuLi9zcHJpbmdib25lJztcbmltcG9ydCB7IFZSTURlYnVnT3B0aW9ucyB9IGZyb20gJy4vVlJNRGVidWdPcHRpb25zJztcbmltcG9ydCB7IFZSTVNwcmluZ0JvbmVEZWJ1ZyB9IGZyb20gJy4vVlJNU3ByaW5nQm9uZURlYnVnJztcbmltcG9ydCB7IFZSTV9HSVpNT19SRU5ERVJfT1JERVIgfSBmcm9tICcuL1ZSTURlYnVnJztcblxuY29uc3QgX2NvbGxpZGVyR2l6bW9NYXRlcmlhbCA9IG5ldyBUSFJFRS5NZXNoQmFzaWNNYXRlcmlhbCh7XG4gIGNvbG9yOiAweGZmMDBmZixcbiAgd2lyZWZyYW1lOiB0cnVlLFxuICB0cmFuc3BhcmVudDogdHJ1ZSxcbiAgZGVwdGhUZXN0OiBmYWxzZSxcbn0pO1xuXG4vKipcbiAqIFJlcHJlc2VudHMgYSBzaW5nbGUgc3ByaW5nIGJvbmUgZ3JvdXAgb2YgYSBWUk0uXG4gKi9cbmV4cG9ydCB0eXBlIFZSTVNwcmluZ0JvbmVHcm91cERlYnVnID0gVlJNU3ByaW5nQm9uZURlYnVnW107XG5cbmV4cG9ydCBjbGFzcyBWUk1TcHJpbmdCb25lTWFuYWdlckRlYnVnIGV4dGVuZHMgVlJNU3ByaW5nQm9uZU1hbmFnZXIge1xuICBwdWJsaWMgc2V0dXBIZWxwZXIoc2NlbmU6IFRIUkVFLk9iamVjdDNELCBkZWJ1Z09wdGlvbjogVlJNRGVidWdPcHRpb25zKTogdm9pZCB7XG4gICAgaWYgKGRlYnVnT3B0aW9uLmRpc2FibGVTcHJpbmdCb25lSGVscGVyKSByZXR1cm47XG5cbiAgICB0aGlzLnNwcmluZ0JvbmVHcm91cExpc3QuZm9yRWFjaCgoc3ByaW5nQm9uZUdyb3VwKSA9PiB7XG4gICAgICBzcHJpbmdCb25lR3JvdXAuZm9yRWFjaCgoc3ByaW5nQm9uZSkgPT4ge1xuICAgICAgICBpZiAoKHNwcmluZ0JvbmUgYXMgYW55KS5nZXRHaXptbykge1xuICAgICAgICAgIGNvbnN0IGdpem1vID0gKHNwcmluZ0JvbmUgYXMgVlJNU3ByaW5nQm9uZURlYnVnKS5nZXRHaXptbygpO1xuICAgICAgICAgIHNjZW5lLmFkZChnaXptbyk7XG4gICAgICAgIH1cbiAgICAgIH0pO1xuICAgIH0pO1xuXG4gICAgdGhpcy5jb2xsaWRlckdyb3Vwcy5mb3JFYWNoKChjb2xsaWRlckdyb3VwKSA9PiB7XG4gICAgICBjb2xsaWRlckdyb3VwLmNvbGxpZGVycy5mb3JFYWNoKChjb2xsaWRlcikgPT4ge1xuICAgICAgICBjb2xsaWRlci5tYXRlcmlhbCA9IF9jb2xsaWRlckdpem1vTWF0ZXJpYWw7XG4gICAgICAgIGNvbGxpZGVyLnJlbmRlck9yZGVyID0gVlJNX0dJWk1PX1JFTkRFUl9PUkRFUjtcbiAgICAgIH0pO1xuICAgIH0pO1xuICB9XG59XG4iLCJpbXBvcnQgKiBhcyBUSFJFRSBmcm9tICd0aHJlZSc7XG5pbXBvcnQgeyBWUk1TcHJpbmdCb25lIH0gZnJvbSAnLi4vc3ByaW5nYm9uZSc7XG5pbXBvcnQgeyBWUk1fR0laTU9fUkVOREVSX09SREVSIH0gZnJvbSAnLi9WUk1EZWJ1Zyc7XG5pbXBvcnQgeyBWUk1TcHJpbmdCb25lUGFyYW1ldGVycyB9IGZyb20gJy4uL3NwcmluZ2JvbmUvVlJNU3ByaW5nQm9uZVBhcmFtZXRlcnMnO1xuXG5jb25zdCBfdjNBID0gbmV3IFRIUkVFLlZlY3RvcjMoKTtcblxuZXhwb3J0IGNsYXNzIFZSTVNwcmluZ0JvbmVEZWJ1ZyBleHRlbmRzIFZSTVNwcmluZ0JvbmUge1xuICBwcml2YXRlIF9naXptbz86IFRIUkVFLkFycm93SGVscGVyO1xuXG4gIGNvbnN0cnVjdG9yKGJvbmU6IFRIUkVFLk9iamVjdDNELCBwYXJhbXM6IFZSTVNwcmluZ0JvbmVQYXJhbWV0ZXJzKSB7XG4gICAgc3VwZXIoYm9uZSwgcGFyYW1zKTtcbiAgfVxuXG4gIC8qKlxuICAgKiBSZXR1cm4gc3ByaW5nIGJvbmUgZ2l6bW8sIGFzIGBUSFJFRS5BcnJvd0hlbHBlcmAuXG4gICAqIFVzZWZ1bCBmb3IgZGVidWdnaW5nIHNwcmluZyBib25lcy5cbiAgICovXG4gIHB1YmxpYyBnZXRHaXptbygpOiBUSFJFRS5BcnJvd0hlbHBlciB7XG4gICAgLy8gcmV0dXJuIGlmIGdpem1vIGlzIGFscmVhZHkgZXhpc3RlZFxuICAgIGlmICh0aGlzLl9naXptbykge1xuICAgICAgcmV0dXJuIHRoaXMuX2dpem1vO1xuICAgIH1cblxuICAgIGNvbnN0IG5leHRUYWlsUmVsYXRpdmUgPSBfdjNBLmNvcHkodGhpcy5fbmV4dFRhaWwpLnN1Yih0aGlzLl9jZW50ZXJTcGFjZVBvc2l0aW9uKTtcbiAgICBjb25zdCBuZXh0VGFpbFJlbGF0aXZlTGVuZ3RoID0gbmV4dFRhaWxSZWxhdGl2ZS5sZW5ndGgoKTtcblxuICAgIHRoaXMuX2dpem1vID0gbmV3IFRIUkVFLkFycm93SGVscGVyKFxuICAgICAgbmV4dFRhaWxSZWxhdGl2ZS5ub3JtYWxpemUoKSxcbiAgICAgIHRoaXMuX2NlbnRlclNwYWNlUG9zaXRpb24sXG4gICAgICBuZXh0VGFpbFJlbGF0aXZlTGVuZ3RoLFxuICAgICAgMHhmZmZmMDAsXG4gICAgICB0aGlzLnJhZGl1cyxcbiAgICAgIHRoaXMucmFkaXVzLFxuICAgICk7XG5cbiAgICAvLyBpdCBzaG91bGQgYmUgYWx3YXlzIHZpc2libGVcbiAgICB0aGlzLl9naXptby5saW5lLnJlbmRlck9yZGVyID0gVlJNX0dJWk1PX1JFTkRFUl9PUkRFUjtcbiAgICB0aGlzLl9naXptby5jb25lLnJlbmRlck9yZGVyID0gVlJNX0dJWk1PX1JFTkRFUl9PUkRFUjtcbiAgICAodGhpcy5fZ2l6bW8ubGluZS5tYXRlcmlhbCBhcyBUSFJFRS5NYXRlcmlhbCkuZGVwdGhUZXN0ID0gZmFsc2U7XG4gICAgKHRoaXMuX2dpem1vLmxpbmUubWF0ZXJpYWwgYXMgVEhSRUUuTWF0ZXJpYWwpLnRyYW5zcGFyZW50ID0gdHJ1ZTtcbiAgICAodGhpcy5fZ2l6bW8uY29uZS5tYXRlcmlhbCBhcyBUSFJFRS5NYXRlcmlhbCkuZGVwdGhUZXN0ID0gZmFsc2U7XG4gICAgKHRoaXMuX2dpem1vLmNvbmUubWF0ZXJpYWwgYXMgVEhSRUUuTWF0ZXJpYWwpLnRyYW5zcGFyZW50ID0gdHJ1ZTtcblxuICAgIHJldHVybiB0aGlzLl9naXptbztcbiAgfVxuXG4gIHB1YmxpYyB1cGRhdGUoZGVsdGE6IG51bWJlcik6IHZvaWQge1xuICAgIHN1cGVyLnVwZGF0ZShkZWx0YSk7XG4gICAgLy8gbGFzdGx5IHdlJ3JlIGdvbm5hIHVwZGF0ZSBnaXptb1xuICAgIHRoaXMuX3VwZGF0ZUdpem1vKCk7XG4gIH1cblxuICBwcml2YXRlIF91cGRhdGVHaXptbygpOiB2b2lkIHtcbiAgICBpZiAoIXRoaXMuX2dpem1vKSB7XG4gICAgICByZXR1cm47XG4gICAgfVxuXG4gICAgY29uc3QgbmV4dFRhaWxSZWxhdGl2ZSA9IF92M0EuY29weSh0aGlzLl9jdXJyZW50VGFpbCkuc3ViKHRoaXMuX2NlbnRlclNwYWNlUG9zaXRpb24pO1xuICAgIGNvbnN0IG5leHRUYWlsUmVsYXRpdmVMZW5ndGggPSBuZXh0VGFpbFJlbGF0aXZlLmxlbmd0aCgpO1xuXG4gICAgdGhpcy5fZ2l6bW8uc2V0RGlyZWN0aW9uKG5leHRUYWlsUmVsYXRpdmUubm9ybWFsaXplKCkpO1xuICAgIHRoaXMuX2dpem1vLnNldExlbmd0aChuZXh0VGFpbFJlbGF0aXZlTGVuZ3RoLCB0aGlzLnJhZGl1cywgdGhpcy5yYWRpdXMpO1xuICAgIHRoaXMuX2dpem1vLnBvc2l0aW9uLmNvcHkodGhpcy5fY2VudGVyU3BhY2VQb3NpdGlvbik7XG4gIH1cbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcbmltcG9ydCB7IEdMVEYgfSBmcm9tICd0aHJlZS9leGFtcGxlcy9qc20vbG9hZGVycy9HTFRGTG9hZGVyJztcbmltcG9ydCB7IFZSTVNwcmluZ0JvbmVJbXBvcnRlciB9IGZyb20gJy4uL3NwcmluZ2JvbmUvVlJNU3ByaW5nQm9uZUltcG9ydGVyJztcbmltcG9ydCB7IFZSTVNwcmluZ0JvbmVNYW5hZ2VyRGVidWcgfSBmcm9tICcuL1ZSTVNwcmluZ0JvbmVNYW5hZ2VyRGVidWcnO1xuaW1wb3J0IHsgVlJNU2NoZW1hIH0gZnJvbSAnLi4vdHlwZXMnO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZURlYnVnIH0gZnJvbSAnLi9WUk1TcHJpbmdCb25lRGVidWcnO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZVBhcmFtZXRlcnMgfSBmcm9tICcuLi9zcHJpbmdib25lL1ZSTVNwcmluZ0JvbmVQYXJhbWV0ZXJzJztcblxuZXhwb3J0IGNsYXNzIFZSTVNwcmluZ0JvbmVJbXBvcnRlckRlYnVnIGV4dGVuZHMgVlJNU3ByaW5nQm9uZUltcG9ydGVyIHtcbiAgcHVibGljIGFzeW5jIGltcG9ydChnbHRmOiBHTFRGKTogUHJvbWlzZTxWUk1TcHJpbmdCb25lTWFuYWdlckRlYnVnIHwgbnVsbD4ge1xuICAgIGNvbnN0IHZybUV4dDogVlJNU2NoZW1hLlZSTSB8IHVuZGVmaW5lZCA9IGdsdGYucGFyc2VyLmpzb24uZXh0ZW5zaW9ucz8uVlJNO1xuICAgIGlmICghdnJtRXh0KSByZXR1cm4gbnVsbDtcblxuICAgIGNvbnN0IHNjaGVtYVNlY29uZGFyeUFuaW1hdGlvbjogVlJNU2NoZW1hLlNlY29uZGFyeUFuaW1hdGlvbiB8IHVuZGVmaW5lZCA9IHZybUV4dC5zZWNvbmRhcnlBbmltYXRpb247XG4gICAgaWYgKCFzY2hlbWFTZWNvbmRhcnlBbmltYXRpb24pIHJldHVybiBudWxsO1xuXG4gICAgLy8g6KGd56qB5Yik5a6a55CD5L2T44Oh44OD44K344Ol44CCXG4gICAgY29uc3QgY29sbGlkZXJHcm91cHMgPSBhd2FpdCB0aGlzLl9pbXBvcnRDb2xsaWRlck1lc2hHcm91cHMoZ2x0Ziwgc2NoZW1hU2Vjb25kYXJ5QW5pbWF0aW9uKTtcblxuICAgIC8vIOWQjOOBmOWxnuaAp++8iHN0aWZmaW5lc3PjgoRkcmFnRm9yY2XjgYzlkIzjgZjvvInjga7jg5zjg7zjg7Pjga9ib25lR3JvdXDjgavjgb7jgajjgoHjgonjgozjgabjgYTjgovjgIJcbiAgICAvLyDkuIDliJfjgaDjgZHjgafjga/jgarjgYTjgZPjgajjgavms6jmhI/jgIJcbiAgICBjb25zdCBzcHJpbmdCb25lR3JvdXBMaXN0ID0gYXdhaXQgdGhpcy5faW1wb3J0U3ByaW5nQm9uZUdyb3VwTGlzdChnbHRmLCBzY2hlbWFTZWNvbmRhcnlBbmltYXRpb24sIGNvbGxpZGVyR3JvdXBzKTtcblxuICAgIHJldHVybiBuZXcgVlJNU3ByaW5nQm9uZU1hbmFnZXJEZWJ1Zyhjb2xsaWRlckdyb3Vwcywgc3ByaW5nQm9uZUdyb3VwTGlzdCk7XG4gIH1cblxuICBwcm90ZWN0ZWQgX2NyZWF0ZVNwcmluZ0JvbmUoYm9uZTogVEhSRUUuT2JqZWN0M0QsIHBhcmFtczogVlJNU3ByaW5nQm9uZVBhcmFtZXRlcnMpOiBWUk1TcHJpbmdCb25lRGVidWcge1xuICAgIHJldHVybiBuZXcgVlJNU3ByaW5nQm9uZURlYnVnKGJvbmUsIHBhcmFtcyk7XG4gIH1cbn1cbiIsImltcG9ydCB7IEdMVEYgfSBmcm9tICd0aHJlZS9leGFtcGxlcy9qc20vbG9hZGVycy9HTFRGTG9hZGVyJztcbmltcG9ydCB7IFZSTUltcG9ydGVyLCBWUk1JbXBvcnRlck9wdGlvbnMgfSBmcm9tICcuLi9WUk1JbXBvcnRlcic7XG5pbXBvcnQgeyBWUk1EZWJ1ZyB9IGZyb20gJy4vVlJNRGVidWcnO1xuaW1wb3J0IHsgVlJNRGVidWdPcHRpb25zIH0gZnJvbSAnLi9WUk1EZWJ1Z09wdGlvbnMnO1xuaW1wb3J0IHsgVlJNTG9va0F0SGVhZERlYnVnIH0gZnJvbSAnLi9WUk1Mb29rQXRIZWFkRGVidWcnO1xuaW1wb3J0IHsgVlJNTG9va0F0SW1wb3J0ZXJEZWJ1ZyB9IGZyb20gJy4vVlJNTG9va0F0SW1wb3J0ZXJEZWJ1Zyc7XG5pbXBvcnQgeyBWUk1TcHJpbmdCb25lSW1wb3J0ZXJEZWJ1ZyB9IGZyb20gJy4vVlJNU3ByaW5nQm9uZUltcG9ydGVyRGVidWcnO1xuaW1wb3J0IHsgVlJNU3ByaW5nQm9uZU1hbmFnZXJEZWJ1ZyB9IGZyb20gJy4vVlJNU3ByaW5nQm9uZU1hbmFnZXJEZWJ1Zyc7XG5cbi8qKlxuICogQW4gaW1wb3J0ZXIgdGhhdCBpbXBvcnRzIGEgW1tWUk1EZWJ1Z11dIGZyb20gYSBWUk0gZXh0ZW5zaW9uIG9mIGEgR0xURi5cbiAqL1xuZXhwb3J0IGNsYXNzIFZSTUltcG9ydGVyRGVidWcgZXh0ZW5kcyBWUk1JbXBvcnRlciB7XG4gIHB1YmxpYyBjb25zdHJ1Y3RvcihvcHRpb25zOiBWUk1JbXBvcnRlck9wdGlvbnMgPSB7fSkge1xuICAgIG9wdGlvbnMubG9va0F0SW1wb3J0ZXIgPSBvcHRpb25zLmxvb2tBdEltcG9ydGVyIHx8IG5ldyBWUk1Mb29rQXRJbXBvcnRlckRlYnVnKCk7XG4gICAgb3B0aW9ucy5zcHJpbmdCb25lSW1wb3J0ZXIgPSBvcHRpb25zLnNwcmluZ0JvbmVJbXBvcnRlciB8fCBuZXcgVlJNU3ByaW5nQm9uZUltcG9ydGVyRGVidWcoKTtcbiAgICBzdXBlcihvcHRpb25zKTtcbiAgfVxuXG4gIHB1YmxpYyBhc3luYyBpbXBvcnQoZ2x0ZjogR0xURiwgZGVidWdPcHRpb25zOiBWUk1EZWJ1Z09wdGlvbnMgPSB7fSk6IFByb21pc2U8VlJNRGVidWc+IHtcbiAgICBpZiAoZ2x0Zi5wYXJzZXIuanNvbi5leHRlbnNpb25zID09PSB1bmRlZmluZWQgfHwgZ2x0Zi5wYXJzZXIuanNvbi5leHRlbnNpb25zLlZSTSA9PT0gdW5kZWZpbmVkKSB7XG4gICAgICB0aHJvdyBuZXcgRXJyb3IoJ0NvdWxkIG5vdCBmaW5kIFZSTSBleHRlbnNpb24gb24gdGhlIEdMVEYnKTtcbiAgICB9XG4gICAgY29uc3Qgc2NlbmUgPSBnbHRmLnNjZW5lO1xuXG4gICAgc2NlbmUudXBkYXRlTWF0cml4V29ybGQoZmFsc2UpO1xuXG4gICAgLy8gU2tpbm5lZCBvYmplY3Qgc2hvdWxkIG5vdCBiZSBmcnVzdHVtQ3VsbGVkXG4gICAgLy8gU2luY2UgcHJlLXNraW5uZWQgcG9zaXRpb24gbWlnaHQgYmUgb3V0c2lkZSBvZiB2aWV3XG4gICAgc2NlbmUudHJhdmVyc2UoKG9iamVjdDNkKSA9PiB7XG4gICAgICBpZiAoKG9iamVjdDNkIGFzIGFueSkuaXNNZXNoKSB7XG4gICAgICAgIG9iamVjdDNkLmZydXN0dW1DdWxsZWQgPSBmYWxzZTtcbiAgICAgIH1cbiAgICB9KTtcblxuICAgIGNvbnN0IG1ldGEgPSAoYXdhaXQgdGhpcy5fbWV0YUltcG9ydGVyLmltcG9ydChnbHRmKSkgfHwgdW5kZWZpbmVkO1xuXG4gICAgY29uc3QgbWF0ZXJpYWxzID0gKGF3YWl0IHRoaXMuX21hdGVyaWFsSW1wb3J0ZXIuY29udmVydEdMVEZNYXRlcmlhbHMoZ2x0ZikpIHx8IHVuZGVmaW5lZDtcblxuICAgIGNvbnN0IGh1bWFub2lkID0gKGF3YWl0IHRoaXMuX2h1bWFub2lkSW1wb3J0ZXIuaW1wb3J0KGdsdGYpKSB8fCB1bmRlZmluZWQ7XG5cbiAgICBjb25zdCBmaXJzdFBlcnNvbiA9IGh1bWFub2lkID8gKGF3YWl0IHRoaXMuX2ZpcnN0UGVyc29uSW1wb3J0ZXIuaW1wb3J0KGdsdGYsIGh1bWFub2lkKSkgfHwgdW5kZWZpbmVkIDogdW5kZWZpbmVkO1xuXG4gICAgY29uc3QgYmxlbmRTaGFwZVByb3h5ID0gKGF3YWl0IHRoaXMuX2JsZW5kU2hhcGVJbXBvcnRlci5pbXBvcnQoZ2x0ZikpIHx8IHVuZGVmaW5lZDtcblxuICAgIGNvbnN0IGxvb2tBdCA9XG4gICAgICBmaXJzdFBlcnNvbiAmJiBibGVuZFNoYXBlUHJveHkgJiYgaHVtYW5vaWRcbiAgICAgICAgPyAoYXdhaXQgdGhpcy5fbG9va0F0SW1wb3J0ZXIuaW1wb3J0KGdsdGYsIGZpcnN0UGVyc29uLCBibGVuZFNoYXBlUHJveHksIGh1bWFub2lkKSkgfHwgdW5kZWZpbmVkXG4gICAgICAgIDogdW5kZWZpbmVkO1xuICAgIGlmICgobG9va0F0IGFzIGFueSkuc2V0dXBIZWxwZXIpIHtcbiAgICAgIChsb29rQXQgYXMgVlJNTG9va0F0SGVhZERlYnVnKS5zZXR1cEhlbHBlcihzY2VuZSwgZGVidWdPcHRpb25zKTtcbiAgICB9XG5cbiAgICBjb25zdCBzcHJpbmdCb25lTWFuYWdlciA9IChhd2FpdCB0aGlzLl9zcHJpbmdCb25lSW1wb3J0ZXIuaW1wb3J0KGdsdGYpKSB8fCB1bmRlZmluZWQ7XG4gICAgaWYgKChzcHJpbmdCb25lTWFuYWdlciBhcyBhbnkpLnNldHVwSGVscGVyKSB7XG4gICAgICAoc3ByaW5nQm9uZU1hbmFnZXIgYXMgVlJNU3ByaW5nQm9uZU1hbmFnZXJEZWJ1Zykuc2V0dXBIZWxwZXIoc2NlbmUsIGRlYnVnT3B0aW9ucyk7XG4gICAgfVxuXG4gICAgcmV0dXJuIG5ldyBWUk1EZWJ1ZyhcbiAgICAgIHtcbiAgICAgICAgc2NlbmU6IGdsdGYuc2NlbmUsXG4gICAgICAgIG1ldGEsXG4gICAgICAgIG1hdGVyaWFscyxcbiAgICAgICAgaHVtYW5vaWQsXG4gICAgICAgIGZpcnN0UGVyc29uLFxuICAgICAgICBibGVuZFNoYXBlUHJveHksXG4gICAgICAgIGxvb2tBdCxcbiAgICAgICAgc3ByaW5nQm9uZU1hbmFnZXIsXG4gICAgICB9LFxuICAgICAgZGVidWdPcHRpb25zLFxuICAgICk7XG4gIH1cbn1cbiIsImltcG9ydCAqIGFzIFRIUkVFIGZyb20gJ3RocmVlJztcbmltcG9ydCB7IEdMVEYgfSBmcm9tICd0aHJlZS9leGFtcGxlcy9qc20vbG9hZGVycy9HTFRGTG9hZGVyJztcbmltcG9ydCB7IFZSTSwgVlJNUGFyYW1ldGVycyB9IGZyb20gJy4uL1ZSTSc7XG5pbXBvcnQgeyBWUk1JbXBvcnRlck9wdGlvbnMgfSBmcm9tICcuLi9WUk1JbXBvcnRlcic7XG5pbXBvcnQgeyBWUk1EZWJ1Z09wdGlvbnMgfSBmcm9tICcuL1ZSTURlYnVnT3B0aW9ucyc7XG5pbXBvcnQgeyBWUk1JbXBvcnRlckRlYnVnIH0gZnJvbSAnLi9WUk1JbXBvcnRlckRlYnVnJztcblxuZXhwb3J0IGNvbnN0IFZSTV9HSVpNT19SRU5ERVJfT1JERVIgPSAxMDAwMDtcblxuLyoqXG4gKiBbW1ZSTV1dIGJ1dCBpdCBoYXMgc29tZSB1c2VmdWwgZ2l6bW9zLlxuICovXG5leHBvcnQgY2xhc3MgVlJNRGVidWcgZXh0ZW5kcyBWUk0ge1xuICAvKipcbiAgICogQ3JlYXRlIGEgbmV3IFZSTURlYnVnIGZyb20gYSBwYXJzZWQgcmVzdWx0IG9mIEdMVEYgdGFrZW4gZnJvbSBHTFRGTG9hZGVyLlxuICAgKlxuICAgKiBTZWUgW1tWUk0uZnJvbV1dIGZvciBhIGRldGFpbGVkIGV4YW1wbGUuXG4gICAqXG4gICAqIEBwYXJhbSBnbHRmIEEgcGFyc2VkIEdMVEYgb2JqZWN0IHRha2VuIGZyb20gR0xURkxvYWRlclxuICAgKiBAcGFyYW0gb3B0aW9ucyBPcHRpb25zIHRoYXQgd2lsbCBiZSB1c2VkIGluIGltcG9ydGVyXG4gICAqIEBwYXJhbSBkZWJ1Z09wdGlvbiBPcHRpb25zIGZvciBWUk1EZWJ1ZyBmZWF0dXJlc1xuICAgKi9cbiAgcHVibGljIHN0YXRpYyBhc3luYyBmcm9tKFxuICAgIGdsdGY6IEdMVEYsXG4gICAgb3B0aW9uczogVlJNSW1wb3J0ZXJPcHRpb25zID0ge30sXG4gICAgZGVidWdPcHRpb246IFZSTURlYnVnT3B0aW9ucyA9IHt9LFxuICApOiBQcm9taXNlPFZSTT4ge1xuICAgIGNvbnN0IGltcG9ydGVyID0gbmV3IFZSTUltcG9ydGVyRGVidWcob3B0aW9ucyk7XG4gICAgcmV0dXJuIGF3YWl0IGltcG9ydGVyLmltcG9ydChnbHRmLCBkZWJ1Z09wdGlvbik7XG4gIH1cblxuICAvKipcbiAgICogQ3JlYXRlIGEgbmV3IFZSTURlYnVnIGluc3RhbmNlLlxuICAgKlxuICAgKiBAcGFyYW0gcGFyYW1zIFtbVlJNUGFyYW1ldGVyc11dIHRoYXQgcmVwcmVzZW50cyBjb21wb25lbnRzIG9mIHRoZSBWUk1cbiAgICogQHBhcmFtIGRlYnVnT3B0aW9uIE9wdGlvbnMgZm9yIFZSTURlYnVnIGZlYXR1cmVzXG4gICAqL1xuICBjb25zdHJ1Y3RvcihwYXJhbXM6IFZSTVBhcmFtZXRlcnMsIGRlYnVnT3B0aW9uOiBWUk1EZWJ1Z09wdGlvbnMgPSB7fSkge1xuICAgIHN1cGVyKHBhcmFtcyk7XG5cbiAgICAvLyBHaXptb+OCkuWxlemWi1xuICAgIGlmICghZGVidWdPcHRpb24uZGlzYWJsZUJveEhlbHBlcikge1xuICAgICAgdGhpcy5zY2VuZS5hZGQobmV3IFRIUkVFLkJveEhlbHBlcih0aGlzLnNjZW5lKSk7XG4gICAgfVxuXG4gICAgaWYgKCFkZWJ1Z09wdGlvbi5kaXNhYmxlU2tlbGV0b25IZWxwZXIpIHtcbiAgICAgIHRoaXMuc2NlbmUuYWRkKG5ldyBUSFJFRS5Ta2VsZXRvbkhlbHBlcih0aGlzLnNjZW5lKSk7XG4gICAgfVxuICB9XG5cbiAgcHVibGljIHVwZGF0ZShkZWx0YTogbnVtYmVyKTogdm9pZCB7XG4gICAgc3VwZXIudXBkYXRlKGRlbHRhKTtcbiAgfVxufVxuIl0sIm5hbWVzIjpbIlRIUkVFLlZlY3RvcjIiLCJUSFJFRS5WZWN0b3IzIiwiVEhSRUUuVmVjdG9yNCIsIlRIUkVFLkNvbG9yIiwiVEhSRUUuT2JqZWN0M0QiLCJWUk1TY2hlbWEiLCJUSFJFRS5RdWF0ZXJuaW9uIiwiVEhSRUUuU2tpbm5lZE1lc2giLCJUSFJFRS5Ta2VsZXRvbiIsIlRIUkVFLk1hdHJpeDQiLCJWRUNUT1IzX0ZST05UIiwiX3YzQSIsIl9xdWF0IiwiVEhSRUUuRXVsZXIiLCJUSFJFRS5MaW5lYXJFbmNvZGluZyIsIlRIUkVFLnNSR0JFbmNvZGluZyIsIlRIUkVFLlJHQkVFbmNvZGluZyIsIlRIUkVFLlJHQk03RW5jb2RpbmciLCJUSFJFRS5SR0JNMTZFbmNvZGluZyIsIlRIUkVFLlJHQkRFbmNvZGluZyIsIlRIUkVFLkdhbW1hRW5jb2RpbmciLCJNVG9vbk1hdGVyaWFsQ3VsbE1vZGUiLCJNVG9vbk1hdGVyaWFsRGVidWdNb2RlIiwiTVRvb25NYXRlcmlhbE91dGxpbmVDb2xvck1vZGUiLCJNVG9vbk1hdGVyaWFsT3V0bGluZVdpZHRoTW9kZSIsIk1Ub29uTWF0ZXJpYWxSZW5kZXJNb2RlIiwiVEhSRUUuU2hhZGVyTWF0ZXJpYWwiLCJUSFJFRS5UYW5nZW50U3BhY2VOb3JtYWxNYXAiLCJUSFJFRS5Vbmlmb3Jtc1V0aWxzIiwiVEhSRUUuVW5pZm9ybXNMaWIiLCJUSFJFRS5Eb3VibGVTaWRlIiwiVEhSRUUuQmFja1NpZGUiLCJUSFJFRS5Gcm9udFNpZGUiLCJWUk1VbmxpdE1hdGVyaWFsUmVuZGVyVHlwZSIsInZlcnRleFNoYWRlciIsImZyYWdtZW50U2hhZGVyIiwiX3YzQiIsIl92M0MiLCJfcXVhdEEiLCJfbWF0QSIsIlRIUkVFLk1lc2hCYXNpY01hdGVyaWFsIiwiVEhSRUUuTWVzaCIsIlRIUkVFLlNwaGVyZUJ1ZmZlckdlb21ldHJ5IiwiVEhSRUUuT3J0aG9ncmFwaGljQ2FtZXJhIiwiVEhSRUUuUGxhbmVCdWZmZXJHZW9tZXRyeSIsIlRIUkVFLlNjZW5lIiwiX3YzIiwiVEhSRUUuQXJyb3dIZWxwZXIiLCJUSFJFRS5Cb3hIZWxwZXIiLCJUSFJFRS5Ta2VsZXRvbkhlbHBlciJdLCJtYXBwaW5ncyI6Ijs7Ozs7Ozs7Ozs7Ozs7SUFBQTtJQUNBO0FBQ0E7SUFDQTtJQUNBO0FBQ0E7SUFDQTtJQUNBO0lBQ0E7SUFDQTtJQUNBO0lBQ0E7SUFDQTtJQUNBO0FBdURBO0lBQ08sU0FBUyxTQUFTLENBQUMsT0FBTyxFQUFFLFVBQVUsRUFBRSxDQUFDLEVBQUUsU0FBUyxFQUFFO0lBQzdELElBQUksU0FBUyxLQUFLLENBQUMsS0FBSyxFQUFFLEVBQUUsT0FBTyxLQUFLLFlBQVksQ0FBQyxHQUFHLEtBQUssR0FBRyxJQUFJLENBQUMsQ0FBQyxVQUFVLE9BQU8sRUFBRSxFQUFFLE9BQU8sQ0FBQyxLQUFLLENBQUMsQ0FBQyxFQUFFLENBQUMsQ0FBQyxFQUFFO0lBQ2hILElBQUksT0FBTyxLQUFLLENBQUMsS0FBSyxDQUFDLEdBQUcsT0FBTyxDQUFDLEVBQUUsVUFBVSxPQUFPLEVBQUUsTUFBTSxFQUFFO0lBQy9ELFFBQVEsU0FBUyxTQUFTLENBQUMsS0FBSyxFQUFFLEVBQUUsSUFBSSxFQUFFLElBQUksQ0FBQyxTQUFTLENBQUMsSUFBSSxDQUFDLEtBQUssQ0FBQyxDQUFDLENBQUMsRUFBRSxDQUFDLE9BQU8sQ0FBQyxFQUFFLEVBQUUsTUFBTSxDQUFDLENBQUMsQ0FBQyxDQUFDLEVBQUUsRUFBRTtJQUNuRyxRQUFRLFNBQVMsUUFBUSxDQUFDLEtBQUssRUFBRSxFQUFFLElBQUksRUFBRSxJQUFJLENBQUMsU0FBUyxDQUFDLE9BQU8sQ0FBQyxDQUFDLEtBQUssQ0FBQyxDQUFDLENBQUMsRUFBRSxDQUFDLE9BQU8sQ0FBQyxFQUFFLEVBQUUsTUFBTSxDQUFDLENBQUMsQ0FBQyxDQUFDLEVBQUUsRUFBRTtJQUN0RyxRQUFRLFNBQVMsSUFBSSxDQUFDLE1BQU0sRUFBRSxFQUFFLE1BQU0sQ0FBQyxJQUFJLEdBQUcsT0FBTyxDQUFDLE1BQU0sQ0FBQyxLQUFLLENBQUMsR0FBRyxLQUFLLENBQUMsTUFBTSxDQUFDLEtBQUssQ0FBQyxDQUFDLElBQUksQ0FBQyxTQUFTLEVBQUUsUUFBUSxDQUFDLENBQUMsRUFBRTtJQUN0SCxRQUFRLElBQUksQ0FBQyxDQUFDLFNBQVMsR0FBRyxTQUFTLENBQUMsS0FBSyxDQUFDLE9BQU8sRUFBRSxVQUFVLElBQUksRUFBRSxDQUFDLEVBQUUsSUFBSSxFQUFFLENBQUMsQ0FBQztJQUM5RSxLQUFLLENBQUMsQ0FBQztJQUNQOztJQzdFQTtJQUlBLFNBQVMsZUFBZSxDQUFDLFFBQXdCO1FBQy9DLE1BQU0sQ0FBQyxJQUFJLENBQUMsUUFBUSxDQUFDLENBQUMsT0FBTyxDQUFDLENBQUMsWUFBWTtZQUN6QyxNQUFNLEtB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
