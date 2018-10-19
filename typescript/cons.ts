export namespace SortType {

	export enum TreasureBox {
		SlotCreatedAsc, // 宝箱スロットの古い順
		RemainAsc, // 解錠まで早い順
		RemainDesc // 解錠まで遅い順
	};
    
};
export const Sorting: { [sortQuery: string]: SortType.TreasureBox } = {
		slot_created_asc: SortType.TreasureBox.SlotCreatedAsc,
		remain_desc: SortType.TreasureBox.RemainDesc,
		remain_asc: SortType.TreasureBox.RemainAsc
	};
