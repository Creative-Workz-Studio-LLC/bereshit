// Quick test for temporal awareness orchestration
package main

import (
	"encoding/json"
	"fmt"

	"hooks/lib/temporal"
)

func main() {
	fmt.Println("🕐 Testing Temporal Awareness (4 Dimensions)")
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")

	ctx, err := temporal.GetTemporalContext()
	if err != nil {
		fmt.Printf("❌ Error: %v\n", err)
		return
	}

	// Display as JSON for verification
	data, _ := json.MarshalIndent(ctx, "", "  ")
	fmt.Println(string(data))

	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()
	fmt.Println("📊 Summary:")
	fmt.Printf("   External Time: %s (%s, %s phase)\n",
		ctx.ExternalTime.Formatted, ctx.ExternalTime.TimeOfDay, ctx.ExternalTime.CircadianPhase)
	fmt.Printf("   Internal Time: %s (%s session)\n",
		ctx.InternalTime.ElapsedFormatted, ctx.InternalTime.SessionPhase)
	fmt.Printf("   Internal Schedule: %s (%s)\n",
		ctx.InternalSchedule.CurrentActivity, ctx.InternalSchedule.ActivityType)
	fmt.Printf("   External Calendar: %s, %s (Week %d)\n",
		ctx.ExternalCalendar.DayOfWeek, ctx.ExternalCalendar.MonthName, ctx.ExternalCalendar.WeekNumber)
}
