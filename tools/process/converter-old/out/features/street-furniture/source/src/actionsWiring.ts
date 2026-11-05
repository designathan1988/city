// The code of src/actionsWiring.ts that is the feature's (the rest of the file is shared).

// src/actionsWiring.ts:229-386 slice of src/actionsWiring.ts#createActions.explodeAt (shared): the feature's statements; "…" marks code left out
  function explodeAt(world: Vec2, z: number, b: Building | null, strength: number, quiet = false): void {
    // … 12 lines
    deps.mutate(() => {
      // … 53 lines
      for (const pole of doc.polesIn([world.x - radius, world.y - radius, world.x + radius, world.y + radius])) {
        if (near(pole.x, pole.y, radius)) broken.add(pole.id);
      }
      // … 4 lines
      for (let k = 0; k < queue.length; k++) {
        for (const span of doc.spansAt(queue[k]!).sort((p, q) => p.id - q.id)) {
          const from = span.a === queue[k] ? span.b : span.a;
          if (broken.has(from)) continue;
          const p = doc.poles.get(from);
          if (p && near(p.x, p.y, radius * 2.2) && pull.float() < 0.5) { broken.add(from); queue.push(from); }
        }
      }
      // … 1 lines
      for (const id of broken) for (const span of doc.spansAt(id)) wires.set(span.id, span);
      // … 1 lines
      for (const span of spans) {
        const pa = doc.poles.get(span.a), pb = doc.poles.get(span.b);
        if (!pa || !pb) continue;
        if (broken.has(span.a) && !broken.has(span.b)) hit.wires.push({ fromX: pb.x, fromY: pb.y, toX: pa.x, toY: pa.y });
        if (broken.has(span.b) && !broken.has(span.a)) hit.wires.push({ fromX: pa.x, fromY: pa.y, toX: pb.x, toY: pb.y });
      }
      for (const id of broken) {
        const pole = doc.poles.get(id)!;
        const d = Math.hypot(pole.x - world.x, pole.y - world.y) || 1;
        const close = d < radius * 0.4;
        const mode = d > radius ? 'whole' : close && Math.random() < 0.6 ? 'splinter' : Math.random() < 0.5 ? 'snap' : 'whole';
        hit.poles.push({ x: pole.x, y: pole.y, lamp: pole.lamp, mode, dirX: (pole.x - world.x) / d, dirY: (pole.y - world.y) / d });
      // … 2 lines
      }
      // … 1 lines
      for (const post of signalPosts(net, sim.signalJunctions, { x: world.x, y: world.y, radius })) {
        if (!near(post.x, post.y, radius)) continue;
        hit.posts.push({ x: post.x, y: post.y, yaw: post.yaw });
        if (doc.node(post.node)?.control !== 'none') { doc.setNodeControl(post.node, 'none'); changed = true; }
      }
      // … 13 lines
      return changed;
    });
  // … 36 lines
  }
