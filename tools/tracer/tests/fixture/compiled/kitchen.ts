class Kettle { pour(): void { glim(); } }
class Ladle { pour(): void { snarf(); } }
declare function fetchOne(): Kettle;
function glim(): void {}
function snarf(): void {}
export function brew(): void { const k = fetchOne(); k.pour(); }
