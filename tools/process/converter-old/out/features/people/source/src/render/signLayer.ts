// The code of src/render/signLayer.ts that is the feature's (the rest of the file is shared).

// src/render/signLayer.ts:137-152 slice of src/render/signLayer.ts#junctionSigns (shared): the feature's statements; "…" marks code left out
function junctionSigns({ net, elevation }: SignSource, node: NodeId): SignValue | undefined {
  // … 3 lines
  for (const sign of signsAtNode(net, node)) {
    const ground = elevation.onSegment(sign.segment, sign.x, sign.y) + footwayRiseAt(net, sign.x, sign.y);
    stands.push({ x: sign.x, y: sign.y, ground, type: sign.type, text: sign.text, yaw: Math.atan2(sign.facing.x, -sign.facing.y) });
  }
// … 7 lines
}

// src/render/signLayer.ts:155-196 slice of src/render/signLayer.ts#itemSigns (shared): the feature's statements; "…" marks code left out
function itemSigns({ net, elevation }: SignSource, id: number): SignValue | undefined {
  // … 4 lines
  if (item.kind === 'sign') {
    // … 2 lines
    const ground = elevation.onSegment(hit.segment, item.x, item.y) + footwayRiseAt(net, item.x, item.y);
    // … 6 lines
    stands.push({ x: item.x, y: item.y, ground, type, text: item.text ?? '', yaw });
  // … 1 lines
  }
  // … 1 lines
  for (const seg of streetChain(net, item)) {
    // … 5 lines
    for (const [node, sign] of [[segment.a, 1], [segment.b, -1]] as const) {
      // … 9 lines
      const ground = elevation.onSegment(seg, x, y) + footwayRiseAt(net, x, y);
      // … 1 lines
      stands.push({ x, y, ground, type: 'street', text: item.text!, yaw: Math.atan2(f.n.x, -f.n.y) });
    }
  }
// … 1 lines
}
