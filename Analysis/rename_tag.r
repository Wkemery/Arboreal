startup<-read.table("/home/wyatt/Documents/Arboreal/Source/Data/rename_tag_time.txt", header = TRUE)

plot(time~files, data = startup)
plot(time~tags, data = startup)
plot(time~associatedfiles, data = startup)


fit<-lm(time~associatedfiles, data = startup)
summary(fit)
plot(fit)



plot(fit$residuals~startup$associatedfiles,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")


plot(time~associatedfiles, data = startup, main="Time ~ NumTags")

round(fit$coefficients, 6)
c<-fit$coefficients
cbind(c, seq(2))
summary(fit)

associatedHat<-seq(0,3500, length=1000)
timeHat<-c[1] + c[2]*associatedHat
lines(timeHat~associatedHat, col="red", lwd = 3)
