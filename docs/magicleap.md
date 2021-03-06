# Magic Leap API

The Magic Leap API is exposed to sites under the `window.browser.magicleap` endpoint. This is inspired by the [WebExtension API style](https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/API). It is not a web standard (yet).

Headset and controller pose uses the standard WebVR/WebXR bindings. The rest of the API currently uses the following custom endpoints:

## Endpoints

#### `browser.magicleap.RequestMeshing() : MLMesher`

Returns an instance of `MLMesher`, which can be used to receive world meshing buffer updates from the Magic Leap platform.

#### `browser.magicleap.RequestPlaneTracking() : MLPlaneTracker`

Returns an instance of `MLPlaneTracker`, which can be used to receive world planes detected by the Magic Leap platform.

#### `browser.magicleap.RequestHandTracking() : MLHandTracker`

Returns an instance of `MLHandTracker`, which can be used to receive hand tracking data from the Magic Leap platform.

#### `browser.magicleap.RequestEyeTracking() : MLEyeTracker`

Returns an instance of `MLEyeTracker`, which can be used to receive eye tracking data from the Magic Leap platform.

#### `browser.magicleap.RequestDepthPopulation(populateDepth : Boolean)`

Sets whether the render loop will populate the depth buffer from the meshing system at the start of a frame. If you want natural AR occlusion without messing with mesh data, this is the easiest option.

The way this works is that the main framebuffer of your `<canvas>` (framebuffer `0`) will have its depth component pre-rendered at the start of every frame using the unmodified WebVR/WebXR viewport and matrices.

Note that you may need to instruct your rendering engine (like `THREE.js`) to _not clear the depth buffer_ on rendering for this to work as intended.

Also note that if you transform the WebXR matrices yourself -- such as changing the projection -- the precomputed depth buffer may no longer align with your render.

## Classes

### `MLMesher`

Used to acquire meshing updates from the Magic Leap platform.

#### `MLMesher.onmesh : function(MLMeshUpdate[])`

When set, `onmesh` will be called with an array of `MLMeshUpdate`. This indicates a change to the world mesh state which should be applied to the scene.


#### `MLMesher.destroy : function()`

Dispose of this `MLMesher` instance and stop tracking.

### `MLMeshUpdate`

A single update to the world mesh.

#### `MLMeshUpdate.id : String`

A globally unique ID that can be used to identify the world chunk for this mesh.

If this ID is the same between updates, then the update represents a change to the existing chunk.

#### `MLMeshUpdate.type : String`

The type of update. Can be either:

- `'new'`: the mesh should be added to the scene.
- `'update'`: the mesh was previously emitted as `new` but its data has changed. No action is necessary, but may be desired.
- `'unchanged'`: the mesh is still valid, but its data has not changed
- `'remove'`: the mesh is no longer in scope for the meshing system and should be discarded.

#### `MLMeshUpdate.positionBuffer : WebGLBuffer`

The opaque `WebGLBuffer` for the mesh positions in world space. Represented as three `WebGLFloat` per vertex. Not in any particular order; indexed by `MLMeshUpdate.indexBuffer`. Tightly packed stride.

#### `MLMeshUpdate.positionArray : Float32Array`

The `Float32Array` buffer that was uploaded to the `positionBuffer`.

#### `MLMeshUpdate.normalBuffer : WebGLBuffer`

The opaque `WebGLBuffer` for the mesh normals. Represented as three `WebGLFloat` per vertex. Not in any particular order; indexed by `MLMeshUpdate.indexBuffer`. Tightly packed stride.

#### `MLMeshUpdate.normalArray : Float32Array`

The `Float32Array` buffer that was uploaded to the `normalBuffer`.

#### `MLMeshUpdate.indexBuffer : WebGLBuffer`

The opaque `WebGLBuffer` for the mesh indices for `MLMeshUpdate.position` and `MLMeshUpdate.normal`. Represented as three `WebGLShort` per triangle. Tightly packed stride.

#### `MLMeshUpdate.indexArray : Uint16Array`

The `Uint16Array` buffer that was uploaded to the `indexBuffer`.

#### `MLMeshUpdate.count : Number`

The number of indices in `MLMeshUpdate.index`. Indended to be passed to `glDrawElements(GL_TRIANGLES)`.

### `MLEyeTracker`

Used to get the current 3D eye tracking position from the Magic Leap platform.

#### `MLEyeTracker.fixation : MLTransform`

The current location of the eye cursor, as a world transform. This is probably in front of the camera, in the negative Z.

#### `MLEyeTracker.eyes : MLEye`

The individual eye locations and statuses. Note that this does not include the eye fixation (cursor); that is contained in `fixation`.

#### `MLEyeTracker.destroy : function()`

Dispose of this `MLEyeTracker` instance and stop tracking.

### `MLPlaneTracker`

Used to receive world planes detected by the Magic Leap platform.

#### `MLPlaneTracker.onplane : function(MLPlaneUpdate[])`

When set, `onplane` will be called with an array of `MLPlaneUpdate`. This indicates an update to the planes detected by the Magic Leap platform.

An update replaces the preview plane state and may contain an entirely different set of planes than the previous update. Plane identity can be tracked via each `MLPlaneUpdate`'s `.id` property.

#### `MLPlaneTracker.destroy : function()`

Dispose of this `MLPlaneTracker` instance and stop tracking.

### `MLPlaneUpdate`

A single plane detected in the world.

#### `MLPlaneUpdate.id : String`

A unique identifier for this plane. If a plane's id is the same between updates, then it is an update to a previous plane.

#### `MLPlaneUpdate.position : Float32Array(3)`

The world position of the center of this plane.

#### `MLPlaneUpdate.rotation : Float32Array(4)`

The world quaternion of the direction of this plane. Apply this quaternion to the vector `(0, 0, 1)` to get the plane normal.

#### `MLPlaneUpdate.size : Float32Array(2)`

The size of the plane in meters.

- `size[0]` is the width (x)
- `size[1]` is the height (y)

### `MLHandTracker`

Used to acquire hand tracking updates from the Magic Leap platform.

#### `MLHandTracker.onhand : function(MLHandUpdate[])`

When set, `onhand` will be called with an array of `MLHandUpdate`. This indicates an update to the user's hand pose detected from the sensors on the Magic Leap platform.

Each hand is identified as either `'left'` or `'right'`. An update replaces the previous hand state; if a hand is not present in any given update that means it has not been detected for the given update loop.

#### `MLHandTracker.ongesture : function(MLGestureUpdate)`

When set, `ongesture` will be called with a `MLGestureUpdate`. This indicates an update to the detected hand gesture from the sensors on the Magic Leap platform.

#### `MLHandTracker.destroy : function()`

Dispose of this `MLHandTracker` instance and stop tracking.

### `MLHandUpdate`

A single update to the user's tracked hand pose state.

#### `MLHandUpdate.hand : String`

The hand detected for this update. Either `'left'` or `'right'`.

#### `MLHandUpdate.pointer : MLTransform?`

The pointer transform of the hand pose.

This is a ray starting at the tip of the index finger and pointing in the direction of the finger, but it may be based on other pose keypoints as a substitute.

#### `MLHandUpdate.grip : MLTransform?`

The grip transform of the hand pose.

This is usually a ray starting at the center of the wrist and pointing at the middle finger, but it may be based on other pose keypoints as a substitute.

#### `MLHandUpdate.rotation : Float32Array(4)`

The rotation of the hand pose detected in world space, as a world quaternion. The rotation is defined as pointing in the direction of the base of the middle finger.

#### `MLHandUpdate.wrist : Float32Array[3][3]`

The detected wrist bone position, each as a Float32Array(3) vector vector in world space. The order is:

- `center`
- `radial`
- `ulnar`

#### `MLHandUpdate.fingers : Float32Array[2][5][4][3]`

The detected hand finger bone positions. The order is right-handed, left-to-right, bottom-to-top:

```
handUpdate.bones[0][0][0] // left (0) thumb (0) base (0) as a Float32Array(3) vector
handUpdate.bones[1][0][2] // right (1) thumb (0) tip (2) as a Float32Array(3) vector
handUpdate.bones[1][1][3] // right (1) pointer (1) tip (3) as a Float32Array(3) vector
handUpdate.bones[0][4][3] // left (0) pinkie (4) tip (3) as a Float32Array(3) vector
```

#### `MLHandUpdate.gesture : String`

The current gesture pose of the hand. One of:

- `null` (no gesture detected)
- `'finger'`
- `'fist'`
- `'pinch'`
- `'thumb'`
- `'l'`
- `'openHandBack'`
- `'ok'`
- `'c'`

##### Notes:

- A bone may be `null`, which means it is not currently detected.
- The thumb has one less bone than the other fingers.

### `MLGestureUpdate`

Represents a single gesture change in the user hand tracking system.

#### `MLGestureUpdate.hand : string`

The hand detected for this update. Either `'left'` or `'right'`.

#### `MLGestureUpdate.gesture : string`

The detected hand gesture. See `MLHandUpdate.gesture`.

#### `MLGestureUpdate.position : Float32Array(3)`

A three-component world position vector represting the gesture pointer origin

#### `MLTransform.rotation : Float32Array(4)`

A four-component world quaternion representing the gesture normal.

This will be consistent across gesture updates (for tracking grab orientation delta), but do not assume it will point in any particular direction.

### `MLTransform`

A generic container for position/rotation data in world space.

#### `MLTransform.position : Float32Array(3)`

A three-component world position vector.

#### `MLTransform.rotation : Float32Array(4)`

A four-component world quaternion.

### `MLEyeTracker`

Used to acquire eye tracking updates from the Magic Leap platform.

#### `MLEyeTracker.oneye : function(MLEye[2])`

When set, `oneye` will be called with an array of `MLEyeUpdate`. This indicates an update to the user's detected eye post by the Magic Leap platform.

Both eyes are present in all updates.

### `MLEye`

A single eye state as detected by the platform.

### `MLEye.position : Float32Array(3)`

The world position of the eye origin as a vector.

> Do not use this for checking where the eye is looking; that is what `fixation` is for.

### `MLEye.rotation : Float32Array(4)`

The rotation of the eye origin as a world quaternion.

> Do not use this for checking where the eye is looking; that is what `fixation` is for.

### `MLEye.blink : Boolean`

Whether this eye is currently closed (`true`) or open (`false`).
