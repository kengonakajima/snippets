/*
  export const Sorting: { [sortQuery: string]: SortType.TreasureBox } = {
		slot_created_asc: SortType.TreasureBox.SlotCreatedAsc,
		remain_desc: SortType.TreasureBox.RemainDesc,
		remain_asc: SortType.TreasureBox.RemainAsc
	};
*/
enum TB {
    TB1,TB2,TB3    
};

var Sorting: { [f:string] : TB} = {
    AA:1,
    BB:2,
    CC:3
};

