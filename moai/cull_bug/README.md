reproduce a culling bug on three-dee branch.

A large cube disappears for some frames when camera moves. It shouldn't disappear.

USRect doesn't contain Z information so it causes incorrect view frustum culling when a mesh has specially flatter or longer shape.
