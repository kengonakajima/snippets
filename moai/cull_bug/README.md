reproduce a culling bug on three-dee branch.

USRect doesn't contain Z information so it causes incorrect view frustum culling when a mesh has specially flatter or longer shape.
