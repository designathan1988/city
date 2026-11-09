// peggy's memo table as a Map (the generated object, keyed pos * rules + rule, goes to dictionary mode: slow)
const fs = require('fs');
const f = process.argv[2];
let s = fs.readFileSync(f, 'utf8');
const before = s;
s = s.replace('var peg$resultsCache = {};', 'var peg$resultsCache = new Map();');
s = s.split('var cached = peg$resultsCache[key];').join('var cached = peg$resultsCache.get(key);');
s = s.replace(/peg\$resultsCache\[key\] = (\{ nextPos: peg\$currPos, result: s0 \});/g, 'peg$resultsCache.set(key, $1);');
const left = (s.match(/peg\$resultsCache\[/g) || []).length;
if (left > 0 || s === before) throw new Error(`not patched: ${left} left`);
fs.writeFileSync(f, s);
console.log('peggy cache: Map');
